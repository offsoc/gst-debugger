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

#include "gstdebugserverqe.h"

#include "common/serializer.h"
#include "common/gstdebugger.pb-c.h"
#include "common/gst-utils.h"

#include <string.h>

typedef struct _QEWatch {
  gint qe_type;
  GstPad * pad;
  gchar * pad_path;
} QEWatch;

static QEWatch * qe_watch_new (gint type, GstPad * pad, gchar * pad_path)
{
  QEWatch * watch = (QEWatch *) g_malloc (sizeof (QEWatch));

  watch->qe_type = type;
  watch->pad = pad;
  watch->pad_path = g_strdup (pad_path);

  return watch;
}

static void qe_watch_free (QEWatch * watch)
{
  g_free (watch->pad_path);
  g_free (watch);
}

static void qe_watch_list_free (gpointer ptr)
{
  g_slist_free_full (ptr, (GDestroyNotify) qe_watch_free);
}

static gint qe_watch_compare (gconstpointer a, gconstpointer b)
{
  QEWatch *a1 = (QEWatch*) a;
  QEWatch *b1 = (QEWatch*) b;

  if (a1->qe_type == b1->qe_type && (g_strcmp0 (a1->pad_path, b1->pad_path) == 0 || a1->pad == NULL)) {
    return 0;
  } else {
    return 1;
  }
}

static gboolean gst_debugserver_qe_ok (GstDebugger__GStreamerData* original, gpointer new_ptr)
{
  GSList *list = new_ptr;
  QEWatch watch;

  watch.pad = NULL;

  if (original->event_info != NULL) {
    watch.qe_type = original->event_info->type;
    watch.pad_path = original->event_info->pad;
  } else {
    watch.qe_type = original->query_info->type;
    watch.pad_path = original->query_info->pad;
  }

  return g_slist_find_custom (list, &watch, qe_watch_compare) != NULL;
}

GstDebugserverQE * gst_debugserver_qe_new (void)
{
  GstDebugserverQE *qe = (GstDebugserverQE*)g_malloc (sizeof(GstDebugserverQE));
  gst_debugserver_watcher_init (&qe->watcher, gst_debugserver_qe_ok, (GDestroyNotify) qe_watch_list_free, qe_watch_compare);

  return qe;
}

void gst_debugserver_qe_free (GstDebugserverQE * qe)
{
  gst_debugserver_qe_clean (qe);
  gst_debugserver_watcher_deinit (&qe->watcher);
  g_free (qe);
}

static gboolean gst_debugserver_qe_add_watch (GstDebugserverQE * qe, gint type,
  GstPad * pad, gchar * pad_path, TcpClient * client)
{
  QEWatch *w = qe_watch_new (type, pad, pad_path);
  if (gst_debugserver_watcher_add_watch (&qe->watcher, w, client) == TRUE) {
    return TRUE;
  } else {
    qe_watch_free (w);
    return FALSE;
  }
}

static gboolean gst_debugserver_qe_remove_watch (GstDebugserverQE * qe,
  gint type, GstPad * pad, gchar * pad_path, TcpClient * client)
{
  QEWatch w = { type, pad, pad_path };

  return gst_debugserver_watcher_remove_watch (&qe->watcher, &w, client);
}

gboolean gst_debugserver_qe_set_watch (GstDebugserverQE * qe, gboolean enable,
  gint type, GstPad * pad, gchar * pad_path, TcpClient * client)
{
  if (enable) {
    return gst_debugserver_qe_add_watch (qe, type, pad, pad_path, client);
  } else {
    return gst_debugserver_qe_remove_watch (qe, type, pad, pad_path, client);
  }
}

void gst_debugserver_qe_send_qe (GstDebugserverQE * qe, GstDebugserverTcp * tcp_server, GstPad * pad, GstMiniObject * obj)
{
  GstDebugger__GStreamerData gst_data = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__EventInfo event_info = GST_DEBUGGER__EVENT_INFO__INIT;
  GstDebugger__QueryInfo query_info = GST_DEBUGGER__QUERY_INFO__INIT;
  gchar *pad_path = gst_utils_get_object_path (GST_OBJECT_CAST (pad));

  if (GST_IS_EVENT (obj)) {
    GstEvent *event = GST_EVENT_CAST (obj);
    event_info.type = event->type;
    event_info.seqnum = event->seqnum;
    event_info.timestamp = event->timestamp;
    event_info.pad = pad_path;
    gst_data.event_info = &event_info;
    gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_EVENT_INFO;
  } else if (GST_IS_QUERY (obj)) {
    GstQuery *query = GST_QUERY_CAST (obj);
    query_info.type = query->type;
    query_info.pad = pad_path;
    gst_data.query_info = &query_info;
    gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_QUERY_INFO;
  }

  gst_debugserver_watcher_send_data (&qe->watcher, tcp_server, &gst_data);

  g_free (pad_path);
}

void gst_debugserver_qe_clean (GstDebugserverQE * qe)
{
  gst_debugserver_watcher_clean (&qe->watcher);
}

void gst_debugserver_qe_remove_client (GstDebugserverQE * qe, TcpClient * client)
{
  g_hash_table_remove (qe->watcher.clients, client);
}
