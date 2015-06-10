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

GstDebugserverMessage * gst_debugserver_message_new (void)
{
  GstDebugserverMessage *msg = (GstDebugserverMessage*)g_malloc (sizeof(GstDebugserverMessage));
  msg->clients = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);

  return msg;
}

void gst_debugserver_message_free (GstDebugserverMessage * msg)
{
  // todo free ghashtable
  g_free (msg);
}

void gst_debugserver_message_add_watch (GstDebugserverMessage * msg,
  GstMessageType msg_type, gpointer client_info)
{
  GList *listeners =
      (GList *) g_hash_table_lookup (msg->clients, g_new(gint, msg_type));

  if (listeners == NULL) {
    listeners = g_list_append (listeners, client_info);
    g_hash_table_insert (msg->clients, g_new(gint, msg_type), listeners);
    return;
  }

  if (g_list_find (listeners, client_info) == NULL) {
    listeners = g_list_append (listeners, client_info);
    g_hash_table_replace (msg->clients, g_new(gint, msg_type),
        listeners);
  }
}

void gst_debugserver_message_remove_watch (GstDebugserverMessage * msg,
  GstMessageType msg_type, gpointer client_info)
{
  GList *listeners =
      (GList *) g_hash_table_lookup (msg->clients, g_new(gint, msg_type));

  listeners = g_list_remove (listeners, client_info);
  g_hash_table_replace (msg->clients, g_new(gint, msg_type), listeners);
}

GList* gst_debugserver_message_get_clients (GstDebugserverMessage * msg,
  GstMessageType msg_type)
{
  return (GList *) g_hash_table_lookup (msg->clients, g_new(gint, msg_type));
}

void gst_debugserver_message_set_watch (GstDebugserverMessage * msg,
  gboolean enable, GstMessageType msg_type, gpointer client_info)
{
  if (enable) {
    gst_debugserver_message_add_watch (msg, msg_type, client_info);
  } else {
    gst_debugserver_message_remove_watch (msg, msg_type, client_info);
  }
}
