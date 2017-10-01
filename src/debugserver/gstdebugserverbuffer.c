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

#include "../common/gst-utils.h"

typedef struct {
  gboolean send_data;
  GstPad * pad;
  gchar * pad_path;
} BufferHook;

static BufferHook * buffer_hook_new (gboolean send_data, GstPad * pad, gchar * pad_path)
{
  BufferHook * hook = (BufferHook *) g_malloc (sizeof (BufferHook));

  hook->send_data = send_data;
  hook->pad = pad;
  hook->pad_path = g_strdup (pad_path);

  return hook;
}

static void buffer_hook_free (BufferHook * hook)
{
  g_free (hook->pad_path);
  g_free (hook);
}

static void buffer_hook_list_free (gpointer ptr)
{
  g_slist_free_full (ptr, (GDestroyNotify) buffer_hook_free);
}

static gint buffer_hook_compare (gconstpointer a, gconstpointer b)
{
  BufferHook *a1 = (BufferHook*) a;
  BufferHook *b1 = (BufferHook*) b;

  if (g_strcmp0 (a1->pad_path, b1->pad_path) == 0 || a1->pad == NULL) {
    return 0;
  } else {
    return 1;
  }
}

GstDebugserverBuffer * gst_debugserver_buffer_new (void)
{
  GstDebugserverBuffer *buf = (GstDebugserverBuffer*)g_malloc (sizeof(GstDebugserverBuffer));

  gst_debugserver_hooks_init (&buf->hooks, NULL, (GDestroyNotify) buffer_hook_list_free, buffer_hook_compare);

  return buf;
}

void gst_debugserver_buffer_free (GstDebugserverBuffer * buf)
{
  gst_debugserver_buffer_clean (buf);
  gst_debugserver_hooks_deinit (&buf->hooks);
  g_free (buf);
}

gboolean gst_debugserver_buffer_add_hook (GstDebugserverBuffer * buf,
  gboolean send_data, GstPad * pad, gchar * pad_path, TcpClient * client)
{
  BufferHook *w = buffer_hook_new (send_data, pad, pad_path);
  if (gst_debugserver_hooks_add_hook (&buf->hooks, w, client) == TRUE) {
    return TRUE;
  } else {
    buffer_hook_free (w);
    return FALSE;
  }
}

gboolean gst_debugserver_buffer_remove_hook (GstDebugserverBuffer * buf,
  gboolean send_data, GstPad * pad, gchar * pad_path, TcpClient * client)
{
  BufferHook w = { send_data, pad, pad_path };

  return gst_debugserver_hooks_remove_hook (&buf->hooks, &w, client);
}

gboolean gst_debugserver_buffer_set_hook (GstDebugserverBuffer * buf, gboolean enable,
  gboolean send_data, GstPad * pad, gchar * pad_path, TcpClient * client)
{
  if (enable) {
    return gst_debugserver_buffer_add_hook (buf, send_data, pad, pad_path, client);
  } else {
    return gst_debugserver_buffer_remove_hook (buf, send_data, pad, pad_path, client);
  }
}

void gst_debugserver_buffer_send_buffer (GstDebugserverBuffer * buffer,
  GstDebugserverTcp * tcp_server, GstPad * pad, GstBuffer * gst_buffer)
{
  GHashTableIter iter;
  gpointer client, value;
  GstDebugger__GStreamerData gst_data = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__BufferInfo buffer_info = GST_DEBUGGER__BUFFER_INFO__INIT;
  gchar *pad_path = gst_utils_get_object_path (GST_OBJECT_CAST (pad));
  GSList *list = NULL;
  BufferHook hook = { FALSE, pad, pad_path };
  guchar *buff_data = NULL;

  buffer_info.dts = GST_BUFFER_DTS (gst_buffer);
  buffer_info.pts = GST_BUFFER_PTS (gst_buffer);
  buffer_info.duration = GST_BUFFER_DURATION (gst_buffer);
  buffer_info.offset = GST_BUFFER_OFFSET (gst_buffer);
  buffer_info.offset = GST_BUFFER_OFFSET_END (gst_buffer);
  buffer_info.pad = pad_path;
  buffer_info.size = gst_buffer_get_size (gst_buffer);

  buff_data = (guchar*) g_malloc (sizeof (guchar) * buffer_info.size);
  gst_buffer_extract (gst_buffer, 0, buff_data, buffer_info.size);

  buffer_info.data.data = buff_data;
  buffer_info.data.len = buffer_info.size;

  gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_BUFFER_INFO;
  gst_data.buffer_info = &buffer_info;

  g_mutex_lock (&buffer->hooks.mutex);
  g_hash_table_iter_init (&iter, buffer->hooks.clients);
  while (g_hash_table_iter_next (&iter, &client, &value)) {
    list = g_slist_find_custom ((GSList*) value, &hook, buffer_hook_compare);
    if (list != NULL) {
      buffer_info.has_data = ((BufferHook*)list->data)->send_data;
      gst_debugserver_tcp_send_packet (tcp_server, (TcpClient*) client, &gst_data);
    }
  }
  g_mutex_unlock (&buffer->hooks.mutex);

  g_free (buff_data);
  g_free (pad_path);
}

void gst_debugserver_buffer_remove_client (GstDebugserverBuffer * buf, TcpClient * client)
{
  gst_debugserver_hooks_remove_client (&buf->hooks, client);
}

void gst_debugserver_buffer_clean (GstDebugserverBuffer * buf)
{
  gst_debugserver_hooks_clean (&buf->hooks);
}
