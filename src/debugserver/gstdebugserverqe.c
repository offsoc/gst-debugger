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
#include <string.h>
#include "../common/gst-utils.h"

typedef struct _QEHook {
  gint qe_type;
  GstPad * pad;
  gchar * pad_path;
} QEHook;

static QEHook * qe_hook_new (gint type, GstPad * pad, gchar * pad_path)
{
  QEHook * hook = (QEHook *) g_malloc (sizeof (QEHook));

  hook->qe_type = type;
  hook->pad = pad;
  hook->pad_path = g_strdup (pad_path);

  return hook;
}

static void qe_hook_free (QEHook * hook)
{
  g_free (hook->pad_path);
  g_free (hook);
}

static void qe_hook_list_free (gpointer ptr)
{
  g_slist_free_full (ptr, (GDestroyNotify) qe_hook_free);
}

static gint qe_hook_compare (gconstpointer a, gconstpointer b)
{
  QEHook *a1 = (QEHook*) a;
  QEHook *b1 = (QEHook*) b;

  if (a1->qe_type == b1->qe_type && (g_strcmp0 (a1->pad_path, b1->pad_path) == 0 || a1->pad == NULL)) {
    return 0;
  } else {
    return 1;
  }
}

static gboolean gst_debugserver_qe_ok (GstDebugger__GStreamerData* original, gpointer new_ptr)
{
  GSList *list = new_ptr;
  QEHook hook;

  hook.pad = NULL;

  if (original->event_info != NULL) {
    hook.qe_type = original->event_info->type;
    hook.pad_path = original->event_info->pad;
  } else {
    hook.qe_type = original->query_info->type;
    hook.pad_path = original->query_info->pad;
  }

  return g_slist_find_custom (list, &hook, qe_hook_compare) != NULL;
}

GstDebugserverQE * gst_debugserver_qe_new (void)
{
  GstDebugserverQE *qe = (GstDebugserverQE*)g_malloc (sizeof(GstDebugserverQE));
  gst_debugserver_hooks_init (&qe->hooks, gst_debugserver_qe_ok, (GDestroyNotify) qe_hook_list_free, qe_hook_compare);

  return qe;
}

void gst_debugserver_qe_free (GstDebugserverQE * qe)
{
  gst_debugserver_qe_clean (qe);
  gst_debugserver_hooks_deinit (&qe->hooks);
  g_free (qe);
}

static gboolean gst_debugserver_qe_add_hook (GstDebugserverQE * qe, gint type,
  GstPad * pad, gchar * pad_path, TcpClient * client)
{
  QEHook *w = qe_hook_new (type, pad, pad_path);
  if (gst_debugserver_hooks_add_hook (&qe->hooks, w, client) == TRUE) {
    return TRUE;
  } else {
    qe_hook_free (w);
    return FALSE;
  }
}

static gboolean gst_debugserver_qe_remove_hook (GstDebugserverQE * qe,
  gint type, GstPad * pad, gchar * pad_path, TcpClient * client)
{
  QEHook w = { type, pad, pad_path };

  return gst_debugserver_hooks_remove_hook (&qe->hooks, &w, client);
}

gboolean gst_debugserver_qe_set_hook (GstDebugserverQE * qe, gboolean enable,
  gint type, GstPad * pad, gchar * pad_path, TcpClient * client)
{
  if (enable) {
    return gst_debugserver_qe_add_hook (qe, type, pad, pad_path, client);
  } else {
    return gst_debugserver_qe_remove_hook (qe, type, pad, pad_path, client);
  }
}

void gst_debugserver_qe_send_qe (GstDebugserverQE * qe, GstDebugserverTcp * tcp_server, GstPad * pad, GstMiniObject * obj)
{
  GstDebugger__GStreamerData gst_data = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__EventInfo event_info = GST_DEBUGGER__EVENT_INFO__INIT;
  GstDebugger__QueryInfo query_info = GST_DEBUGGER__QUERY_INFO__INIT;
  gchar *pad_path = gst_utils_get_object_path (GST_OBJECT_CAST (pad));
  const GstStructure *structure = NULL;

  if (GST_IS_EVENT (obj)) {
    GstEvent *event = GST_EVENT_CAST (obj);
    event_info.type = event->type;
    event_info.seqnum = event->seqnum;
    event_info.timestamp = event->timestamp;
    structure = gst_event_get_structure (event);
    if (structure == NULL) {
      event_info.structure_data.data = NULL;
      event_info.structure_data.len = 0;
    } else {
      event_info.structure_data.data = gst_structure_to_string (structure);
      event_info.structure_data.len = strlen (event_info.structure_data.data);
    }
    event_info.pad = pad_path;
    gst_data.event_info = &event_info;
    gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_EVENT_INFO;
  } else if (GST_IS_QUERY (obj)) {
    GstQuery *query = GST_QUERY_CAST (obj);
    query_info.type = query->type;
    query_info.pad = pad_path;
    structure = gst_query_get_structure (query);
    if (structure == NULL) {
      query_info.structure_data.data = NULL;
      query_info.structure_data.len = 0;
    } else {
      query_info.structure_data.data = gst_structure_to_string (structure);
      query_info.structure_data.len = strlen (query_info.structure_data.data);
    }
    gst_data.query_info = &query_info;
    gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_QUERY_INFO;
  }

  gst_debugserver_hooks_send_data (&qe->hooks, tcp_server, &gst_data);

  if (GST_IS_EVENT (obj)) {
    g_free (event_info.structure_data.data);
  } else if (GST_IS_QUERY (obj)) {
    g_free (query_info.structure_data.data);
  }

  g_free (pad_path);
}

void gst_debugserver_qe_clean (GstDebugserverQE * qe)
{
  gst_debugserver_hooks_clean (&qe->hooks);
}

void gst_debugserver_qe_remove_client (GstDebugserverQE * qe, TcpClient * client)
{
  gst_debugserver_hooks_remove_client (&qe->hooks, client);
}
