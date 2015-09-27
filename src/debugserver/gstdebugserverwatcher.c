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

#include "gstdebugserverwatcher.h"

void gst_debugserver_watcher_init (GstDebugserverWatcher * watcher, OkFunction ok_function,
  GDestroyNotify hash_destroy, GCompareFunc cmp_func)
{
  watcher->clients = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, hash_destroy);
  g_mutex_init (&watcher->mutex);
  watcher->ok_function = ok_function;
  watcher->cmp_function = cmp_func;
}

void gst_debugserver_watcher_deinit (GstDebugserverWatcher * watcher)
{
  g_mutex_lock (&watcher->mutex);
  g_hash_table_destroy (watcher->clients);
  g_mutex_unlock (&watcher->mutex);
}

void gst_debugserver_watcher_clean (GstDebugserverWatcher * watcher)
{
  g_mutex_lock (&watcher->mutex);
  g_hash_table_remove_all (watcher->clients);
  g_mutex_unlock (&watcher->mutex);
}

void gst_debugserver_watcher_send_data (GstDebugserverWatcher * watcher, GstDebugserverTcp * tcp_server, GstDebugger__GStreamerData * gst_data)
{
  GHashTableIter iter;
  gpointer client, value;

  g_mutex_lock (&watcher->mutex);
  g_hash_table_iter_init (&iter, watcher->clients);
  while (g_hash_table_iter_next (&iter, &client, &value)) {
    if (watcher->ok_function (gst_data, value))
      gst_debugserver_tcp_send_packet (tcp_server, (TcpClient*) client, gst_data);
  }
  g_mutex_unlock (&watcher->mutex);
}

gboolean gst_debugserver_watcher_add_watch (GstDebugserverWatcher * watcher, gpointer data, TcpClient * client)
{
  GSList *listeners =
      (GSList *) g_hash_table_lookup (watcher->clients, client);

  if (listeners == NULL) {
    listeners = g_slist_append (listeners, data);
    g_hash_table_insert (watcher->clients, client, listeners);
    return TRUE;
  }

  if (g_slist_find_custom (listeners, data, watcher->cmp_function) == NULL) {
    listeners = g_slist_append (listeners, data);
    g_hash_table_steal (watcher->clients, client);
    g_hash_table_insert (watcher->clients, client, listeners);
    return TRUE;
  } else {
    return FALSE;
  }
}

gboolean gst_debugserver_watcher_remove_watch (GstDebugserverWatcher * watcher,
  gpointer data, TcpClient * client)
{
  GSList *listeners =
      (GSList *) g_hash_table_lookup (watcher->clients, client);

  GSList *l = g_slist_find_custom (listeners, data, watcher->cmp_function);
  if (l == NULL) {
    return FALSE;
  } else {
    listeners = g_slist_remove_link (listeners, l);
    g_hash_table_steal (watcher->clients, client);
    g_hash_table_insert (watcher->clients, client, listeners);
    return TRUE;
  }
}
