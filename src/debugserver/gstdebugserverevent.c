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

#include "gstdebugserverevent.h"
#include "protocol/serializer.h"
#include "protocol/gstdebugger.pb-c.h"

#include <string.h>

GstDebugserverEvent * gst_debugserver_event_new (void)
{
  GstDebugserverEvent *evt = (GstDebugserverEvent*)g_malloc (sizeof(GstDebugserverEvent));
  evt->watches = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);

  return evt;
}

static void gst_debugserver_event_clean_client (gpointer key, gpointer value,
  gpointer user_data)
{
  g_slist_free_full (value, g_free);
}

void gst_debugserver_event_free (GstDebugserverEvent * evt)
{
  g_hash_table_foreach (evt->watches, gst_debugserver_event_clean_client, NULL);
  g_hash_table_destroy (evt->watches);
  g_free (evt);
}

gint gst_debugserver_event_prepare_buffer (GstEvent * event, gchar * buffer, gint max_size)
{
  gchar buff[1024];
  // todo verify max_size
  gint size = gst_event_serialize (event, buff, max_size);
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  GstreamerEvent evt = GSTREAMER_EVENT__INIT;
  evt.payload.len = size;
  evt.payload.data = (uint8_t*)g_malloc (size);
  memcpy (evt.payload.data, buff, size);
  info.info_type = GSTREAMER_INFO__INFO_TYPE__EVENT;
  info.event = &evt;

  assert(size <= max_size);
  gstreamer_info__pack (&info, (guint8*)buffer);
  return size;
}

static void gst_debugserver_event_append_client (gpointer key, gpointer value,
  gpointer user_data)
{
  GArray *tmp = (GArray *) user_data;
  GSList **clients = g_array_index (tmp, GSList**, 0);
  GstEvent *event = g_array_index (tmp, GstEvent*, 1);
  GstPad *pad = g_array_index (tmp, GstPad*, 2);
  GSList * watches = (GSList*) value;
  EventWatch *watch;
  gboolean pad_ok, event_type_ok;

  while (watches != NULL) {
    watch = (EventWatch*)watches->data;
    pad_ok = watch->pad == NULL || watch->pad == pad;
    event_type_ok = watch->event_type == -1 ||
      GST_EVENT_TYPE (event) == (guint) watch->event_type;

    if (pad_ok && event_type_ok) {
      *clients = g_slist_append (*clients, key);
      break;
    }
    watches = watches->next;
  }
}

GSList* gst_debugserver_event_get_clients (GstDebugserverEvent * evt, GstPad * pad,
  GstEvent * event)
{
  GSList * clients = NULL;
  GSList ** ptr_clients = &clients;
  GArray *tmp = g_array_new (FALSE, FALSE, sizeof (gpointer));
  g_array_insert_val (tmp, 0, ptr_clients);
  g_array_insert_val (tmp, 1, event);
  g_array_insert_val (tmp, 2, pad);

  g_hash_table_foreach (evt->watches, gst_debugserver_event_append_client, tmp);
  g_array_unref (tmp);

  return clients;
}

static gint event_watch_compare (gconstpointer a, gconstpointer b)
{
  EventWatch *a1 = (EventWatch*) a;
  EventWatch *b1 = (EventWatch*) b;

  if (a1->event_type == b1->event_type && a1->pad == b1->pad) {
    return 0;
  } else {
    return 1;
  }
}

void gst_debugserver_event_set_watch (GstDebugserverEvent * evt, gboolean enable,
  GstPad * pad, gint event_type, gpointer client_info)
{
  if (enable == TRUE) {
    EventWatch *watch = g_malloc (sizeof (EventWatch));
    watch->event_type = event_type;
    watch->pad = pad;
    GSList *watches = g_hash_table_lookup (evt->watches, client_info);
    if (watches == NULL) {
      watches = g_slist_append (watches, watch);
      g_hash_table_insert (evt->watches, client_info, watches);
      return;
    } else if (g_slist_find_custom (watches, watch, event_watch_compare) != NULL) {
      return;
    }

    watches = g_slist_append (watches, watch);
    g_hash_table_replace (evt->watches, client_info, watches);
  } else {
    GSList *list = g_hash_table_lookup (evt->watches, client_info);
    if (list == NULL) {
      return;
    }
    EventWatch w; w.pad = pad; w.event_type = event_type;
    GSList *link = g_slist_find_custom (list, &w, event_watch_compare);
    g_free (link->data);
    list = g_slist_delete_link (list, link);
    g_hash_table_replace (evt->watches, client_info, list);
  }
}
