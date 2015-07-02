/* GStreamer
 * Copyright (C) 2015 Marcin Kolny <marcin.kolny@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "protocol/serializer.h"
#include "protocol/gstdebugger.pb-c.h"

#include <string.h>
#include "gstdebugserverqe.h"

GstDebugserverQE * gst_debugserver_qe_new (void)
{
  GstDebugserverQE *qe = (GstDebugserverQE*)g_malloc (sizeof(GstDebugserverQE));
  qe->watches = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);

  return qe;
}

static void gst_debugserver_qe_clean_client (gpointer key, gpointer value,
  gpointer user_data)
{
  g_slist_free_full (value, g_free);
}

void gst_debugserver_qe_free (GstDebugserverQE * qe)
{
  g_hash_table_foreach (qe->watches, gst_debugserver_qe_clean_client, NULL);
  g_hash_table_destroy (qe->watches);
  g_free (qe);
}

gint gst_debugserver_qe_prepare_confirmation_buffer (gchar * pad_path, gint qe_type,
  gboolean toggle, gchar * buffer, gint max_size, PadWatch__WatchType type)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  PadWatch pad_watch = PAD_WATCH__INIT;
  gint size;
  pad_watch.pad_path = g_strdup (pad_path);
  pad_watch.toggle = toggle;
  pad_watch.watch_type = type;
  pad_watch.has_qe_type = 1;
  pad_watch.qe_type = qe_type;
  info.confirmation = &pad_watch;
  info.info_type = GSTREAMER_INFO__INFO_TYPE__PAD_WATCH_CONFIRMATION;

  size = gstreamer_info__get_packed_size (&info);
  assert(size <= max_size);
  gstreamer_info__pack (&info, (guint8*)buffer);
  return size;
}

gint gst_debugserver_qeb_prepare_buffer (GstMiniObject * miniobj, gchar * buffer, gint max_size)
{
  gchar buff[1024];
  // todo verify max_size
  gint size;
  GstreamerInfo__InfoType info_type;
  if (GST_IS_QUERY (miniobj)) {
    size = gst_query_serialize (GST_QUERY (miniobj), buff, max_size);
    info_type = GSTREAMER_INFO__INFO_TYPE__QUERY;
  } else if (GST_IS_EVENT (miniobj)) {
    size = gst_event_serialize (GST_EVENT (miniobj), buff, max_size);
    info_type = GSTREAMER_INFO__INFO_TYPE__EVENT;
  }
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  GstreamerQEB evt = GSTREAMER_QEB__INIT;
  evt.payload.len = size;
  evt.payload.data = (uint8_t*)g_malloc (size);
  memcpy (evt.payload.data, buff, size);
  info.info_type = info_type;
  info.qeb = &evt;
  size = gstreamer_info__get_packed_size (&info);
  assert(size <= max_size);
  gstreamer_info__pack (&info, (guint8*)buffer);
  return size;
}

static void gst_debugserver_qe_append_client (gpointer key, gpointer value,
  gpointer user_data)
{
  GArray *tmp = (GArray *) user_data;
  GSList **clients = g_array_index (tmp, GSList**, 0);
  gint type = g_array_index (tmp, gint, 1);
  GstPad *pad = g_array_index (tmp, GstPad*, 2);
  GSList * watches = (GSList*) value;
  QEWatch *watch;
  gboolean pad_ok, type_ok;

  while (watches != NULL) {
    watch = (QEWatch*)watches->data;
    pad_ok = watch->pad == NULL || watch->pad == pad;
    type_ok = watch->qe_type == -1 ||
      type == watch->qe_type;

    if (pad_ok && type_ok) {
      *clients = g_slist_append (*clients, key);
      break;
    }
    watches = watches->next;
  }
}

GSList* gst_debugserver_qe_get_clients (GstDebugserverQE * evt, GstPad * pad,
  gint type)
{
  GSList * clients = NULL;
  GSList ** ptr_clients = &clients;
  GArray *tmp = g_array_new (FALSE, FALSE, sizeof (gpointer));
  g_array_insert_val (tmp, 0, ptr_clients);
  g_array_insert_val (tmp, 1, type);
  g_array_insert_val (tmp, 2, pad);

  g_hash_table_foreach (evt->watches, gst_debugserver_qe_append_client, tmp);
  g_array_unref (tmp);

  return clients;
}

static gint qe_watch_compare (gconstpointer a, gconstpointer b)
{
  QEWatch *a1 = (QEWatch*) a;
  QEWatch *b1 = (QEWatch*) b;

  if (a1->qe_type == b1->qe_type && a1->pad == b1->pad) {
    return 0;
  } else {
    return 1;
  }
}

gboolean gst_debugserver_qe_set_watch (GstDebugserverQE * evt, gboolean enable,
  GstPad * pad, gint type, gpointer client_info)
{
  if (enable == TRUE) {
	  QEWatch *watch = g_malloc (sizeof (QEWatch));
    watch->qe_type = type;
    watch->pad = pad;
    GSList *watches = g_hash_table_lookup (evt->watches, client_info);
    if (watches == NULL) {
      watches = g_slist_append (watches, watch);
      g_hash_table_insert (evt->watches, client_info, watches);
      return TRUE;
    } else if (g_slist_find_custom (watches, watch, qe_watch_compare) != NULL) {
      return FALSE;
    }

    watches = g_slist_append (watches, watch);
    g_hash_table_replace (evt->watches, client_info, watches);
    return TRUE;
  } else {
    GSList *list = g_hash_table_lookup (evt->watches, client_info);
    if (list == NULL) {
      return FALSE;
    }
    QEWatch w; w.pad = pad; w.qe_type = type;
    GSList *link = g_slist_find_custom (list, &w, qe_watch_compare);
    if (link == NULL) {
      return FALSE;
    }
    g_free (link->data);
    list = g_slist_delete_link (list, link);
    g_hash_table_replace (evt->watches, client_info, list);
    return TRUE;
  }
}
