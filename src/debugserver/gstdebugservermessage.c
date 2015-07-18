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

#include <assert.h>

GstDebugserverMessage * gst_debugserver_message_new (void)
{
  GstDebugserverMessage *msg = (GstDebugserverMessage*)g_malloc (sizeof(GstDebugserverMessage));
  msg->clients = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL,
    (GDestroyNotify) g_slist_free);

  return msg;
}

void gst_debugserver_message_free (GstDebugserverMessage * msg)
{
  g_hash_table_unref (msg->clients);
  g_free (msg);
}

gboolean gst_debugserver_message_add_watch (GstDebugserverMessage * msg,
  GstMessageType msg_type, gpointer client_info)
{
  GSList *listeners =
      (GSList *) g_hash_table_lookup (msg->clients, GINT_TO_POINTER (msg_type));

  if (listeners == NULL) {
    listeners = g_slist_append (listeners, client_info);
    g_hash_table_insert (msg->clients, GINT_TO_POINTER (msg_type), listeners);
    return TRUE;
  }

  if (g_slist_find (listeners, client_info) == NULL) {
    listeners = g_slist_append (listeners, client_info);
    g_hash_table_replace (msg->clients, GINT_TO_POINTER (msg_type),
        listeners);
    return TRUE;
  } else {
    return FALSE;
  }
}

gboolean gst_debugserver_message_remove_watch (GstDebugserverMessage * msg,
  GstMessageType msg_type, gpointer client_info)
{
  GSList *listeners =
      (GSList *) g_hash_table_lookup (msg->clients, GINT_TO_POINTER (msg_type));

  if (g_slist_find (listeners, client_info) == NULL) {
    return FALSE;
  } else {
    listeners = g_slist_remove (listeners, client_info);
    g_hash_table_replace (msg->clients, GINT_TO_POINTER (msg_type), listeners);
    return TRUE;
  }
}

GSList* gst_debugserver_message_get_clients (GstDebugserverMessage * msg,
  GstMessageType msg_type)
{
  GSList *base = (GSList *) g_hash_table_lookup (msg->clients, GINT_TO_POINTER (msg_type));
  GSList *clients = g_slist_copy (base);
  base = (GSList *) g_hash_table_lookup (msg->clients, GINT_TO_POINTER(GST_MESSAGE_ANY));

  for (; base != NULL; base = g_slist_next (base)) {
    if (g_slist_find (clients, base->data) == NULL) {
      clients = g_slist_append(clients, base->data);
    }
  }

  return clients;
}

gboolean gst_debugserver_message_set_watch (GstDebugserverMessage * msg,
  gboolean enable, GstMessageType msg_type, gpointer client_info)
{
  if (enable) {
    return gst_debugserver_message_add_watch (msg, msg_type, client_info);
  } else {
    return gst_debugserver_message_remove_watch (msg, msg_type, client_info);
  }
}

gint gst_debugserver_message_prepare_confirmation_buffer (MessageWatch *watch,
  gchar * buffer, gint max_size)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  gint size;
  MessageWatch msg_watch = MESSAGE_WATCH__INIT;

  info.info_type = GSTREAMER_INFO__INFO_TYPE__MESSAGE_CONFIRMATION;
  msg_watch.message_type = watch->message_type;
  msg_watch.toggle = watch->toggle;
  info.bus_msg_confirmation = &msg_watch;
  size = gstreamer_info__get_packed_size (&info);
  assert(size <= max_size);
  gstreamer_info__pack (&info, (guint8*)buffer);

  return size;
}

void gst_debugserver_message_clean (GstDebugserverMessage * msg)
{
  g_hash_table_remove_all (msg->clients);
}
