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

#include "gstdebugserverbuffer.h"

GstDebugserverBuffer * gst_debugserver_buffer_new (void)
{
  GstDebugserverBuffer *buf = (GstDebugserverBuffer*)g_malloc (sizeof(GstDebugserverBuffer));
  buf->clients = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL,
    (GDestroyNotify) g_slist_free);

  return buf;
}

void gst_debugserver_buffer_free (GstDebugserverBuffer * buf)
{
  g_hash_table_unref (buf->clients);
  g_free (buf);
}

gboolean gst_debugserver_buffer_add_watch (GstDebugserverBuffer * buf,
  GstPad * pad, gpointer client_info)
{
  GSList *listeners =
      (GSList *) g_hash_table_lookup (buf->clients, pad);

  if (listeners == NULL) {
    listeners = g_slist_append (listeners, client_info);
    g_hash_table_insert (buf->clients, pad, listeners);
    return TRUE;
  }

  if (g_slist_find (listeners, client_info) == NULL) {
    listeners = g_slist_append (listeners, client_info);
    g_hash_table_replace (buf->clients, pad,
        listeners);
    return TRUE;
  } else {
    return FALSE;
  }
}

gboolean gst_debugserver_buffer_remove_watch (GstDebugserverBuffer * buf,
  GstPad * pad, gpointer client_info)
{
  GSList *listeners =
      (GSList *) g_hash_table_lookup (buf->clients, pad);

  if (g_slist_find (listeners, client_info) == NULL) {
    return FALSE;
  } else {
    listeners = g_slist_remove (listeners, client_info);
    g_hash_table_replace (buf->clients, pad, listeners);
    return TRUE;
  }
}

GSList* gst_debugserver_buffer_get_clients (GstDebugserverBuffer * buf,
  GstPad * pad)
{
  GSList *base = (GSList *) g_hash_table_lookup (buf->clients, pad);
  GSList *clients = g_slist_copy (base);
  base = (GSList *) g_hash_table_lookup (buf->clients, NULL);

  for (; base != NULL; base = g_slist_next (base)) {
    if (g_slist_find (clients, base->data) == NULL) {
      clients = g_slist_append(clients, base->data);
    }
  }

  return clients;
}

gboolean gst_debugserver_buffer_set_watch (GstDebugserverBuffer * buf,
  gboolean enable, GstPad * pad, gpointer client_info)
{
  if (enable) {
    return gst_debugserver_buffer_add_watch (buf, pad, client_info);
  } else {
    return gst_debugserver_buffer_remove_watch (buf, pad, client_info);
  }
}

void gst_debugserver_buffer_remove_client (GstDebugserverBuffer * buf, gpointer client_info)
{
  GList *list = g_hash_table_get_keys (buf->clients);
  GList *free_list = list;

  while (list) {
    gst_debugserver_buffer_remove_watch (buf, GST_PAD_CAST (list->data), client_info);
    list = g_list_next (list);
  }

  g_list_free (free_list);
}

void gst_debugserver_buffer_clean (GstDebugserverBuffer * buf)
{
  g_hash_table_remove_all (buf->clients);
}
