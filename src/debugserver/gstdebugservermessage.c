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

#include "gstdebugservermessage.h"

#include <string.h>
#include <assert.h>

static gint g_int_cmp (gconstpointer v1, gconstpointer v2)
{
  return GPOINTER_TO_INT (v1) != GPOINTER_TO_INT (v2);
}

static gboolean gst_debugserver_message_ok (GstDebugger__GStreamerData* original, gpointer new_ptr)
{
  GSList *list = new_ptr;
  GstDebugger__MessageInfo *msg = original->message_info;

  while (list) {
    if (msg->type == GPOINTER_TO_INT (list->data) || GPOINTER_TO_INT (list->data) == GST_MESSAGE_ANY) {
      return TRUE;
    }
    list = g_slist_next (list);
  }

  return FALSE;
}

GstDebugserverMessage * gst_debugserver_message_new (void)
{
  GstDebugserverMessage *msg = (GstDebugserverMessage*)g_malloc (sizeof(GstDebugserverMessage));

  gst_debugserver_hooks_init (&msg->hooks, gst_debugserver_message_ok, (GDestroyNotify) g_slist_free, g_int_cmp);

  return msg;
}

void gst_debugserver_message_free (GstDebugserverMessage * msg)
{
  gst_debugserver_log_clean (msg);
  gst_debugserver_hooks_deinit (&msg->hooks);
  g_free (msg);
}

void gst_debugserver_message_clean (GstDebugserverMessage * msg)
{
  gst_debugserver_hooks_clean (&msg->hooks);
}

gboolean gst_debugserver_message_set_hook (GstDebugserverMessage * msg,
  TcpClient * client, gboolean enable, GstDebugger__MessageRequest * request)
{
  if (enable) {
    return gst_debugserver_hooks_add_hook (&msg->hooks, GINT_TO_POINTER (request->type), client);
  } else {
    return gst_debugserver_hooks_remove_hook (&msg->hooks, GINT_TO_POINTER (request->type), client);
  }
}

void gst_debugserver_message_remove_client (GstDebugserverMessage * msg,
  TcpClient * client)
{
  gst_debugserver_hooks_remove_client (&msg->hooks, client);
}


void gst_debugserver_message_send_message (GstDebugserverMessage * msg, GstDebugserverTcp * tcp_server,
  GstMessage * gst_msg)
{
  GstDebugger__GStreamerData gst_data = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__MessageInfo msg_info = GST_DEBUGGER__MESSAGE_INFO__INIT;
  const GstStructure *msg_structure = gst_message_get_structure (gst_msg);
  gchar *structure_data = msg_structure != NULL ? gst_structure_to_string (msg_structure) : NULL;

  msg_info.seqnum = gst_msg->seqnum;
  msg_info.timestamp = gst_msg->timestamp;
  msg_info.type = gst_msg->type;
  msg_info.structure_data.data = structure_data;
  msg_info.structure_data.len = structure_data == NULL ? 0 : strlen (structure_data);

  gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_MESSAGE_INFO;
  gst_data.message_info = &msg_info;

  gst_debugserver_hooks_send_data (&msg->hooks, tcp_server, &gst_data);

  g_free (structure_data);
}
