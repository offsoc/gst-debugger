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

#include "gstdebugserverhooks.h"

void
gst_debugserver_hooks_init (GstDebugserverHooks * hooks, OkFunction ok_function,
    GDestroyNotify hash_destroy, GCompareFunc cmp_func)
{
  hooks->clients =
      g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, hash_destroy);
  g_mutex_init (&hooks->mutex);
  hooks->ok_function = ok_function;
  hooks->cmp_function = cmp_func;
}

void
gst_debugserver_hooks_deinit (GstDebugserverHooks * hooks)
{
  g_mutex_lock (&hooks->mutex);
  g_hash_table_destroy (hooks->clients);
  g_mutex_unlock (&hooks->mutex);
  g_mutex_clear (&hooks->mutex);
}

void
gst_debugserver_hooks_send_data (GstDebugserverHooks * hooks,
    GstDebugserverTcp * tcp_server, GstDebugger__GStreamerData * gst_data)
{
  GHashTableIter iter;
  gpointer client, value;

  g_mutex_lock (&hooks->mutex);
  g_hash_table_iter_init (&iter, hooks->clients);
  while (g_hash_table_iter_next (&iter, &client, &value)) {
    if (hooks->ok_function (gst_data, value))
      gst_debugserver_tcp_send_packet (tcp_server, (TcpClient *) client,
          gst_data);
  }
  g_mutex_unlock (&hooks->mutex);
}

gboolean
gst_debugserver_hooks_add_hook (GstDebugserverHooks * hooks, gpointer data,
    TcpClient * client)
{
  GSList *listeners;
  gboolean ret = FALSE;

  g_mutex_lock (&hooks->mutex);
  listeners = (GSList *) g_hash_table_lookup (hooks->clients, client);

  if (listeners == NULL) {
    listeners = g_slist_append (listeners, data);
    g_hash_table_insert (hooks->clients, client, listeners);
    ret = TRUE;
    goto finalize;
  }

  if (g_slist_find_custom (listeners, data, hooks->cmp_function) == NULL) {
    listeners = g_slist_append (listeners, data);
    g_hash_table_steal (hooks->clients, client);
    g_hash_table_insert (hooks->clients, client, listeners);
    ret = TRUE;
    goto finalize;
  }

finalize:
  g_mutex_unlock (&hooks->mutex);
  return ret;
}

gboolean
gst_debugserver_hooks_remove_hook (GstDebugserverHooks * hooks,
    gpointer data, TcpClient * client)
{
  GSList *listeners;

  g_mutex_lock (&hooks->mutex);
  listeners = (GSList *) g_hash_table_lookup (hooks->clients, client);

  GSList *l = g_slist_find_custom (listeners, data, hooks->cmp_function);
  if (l == NULL) {
    g_mutex_unlock (&hooks->mutex);
    return FALSE;
  } else {
    listeners = g_slist_remove_link (listeners, l);
    g_hash_table_steal (hooks->clients, client);
    g_hash_table_insert (hooks->clients, client, listeners);
    g_mutex_unlock (&hooks->mutex);
    return TRUE;
  }
}

void
gst_debugserver_hooks_remove_client (GstDebugserverHooks * hooks,
    TcpClient * client)
{
  g_mutex_lock (&hooks->mutex);
  g_hash_table_remove (hooks->clients, client);
  g_mutex_unlock (&hooks->mutex);
}
