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

#include "gstdebugserverlog.h"
#include "protocol/gstdebugger.pb-c.h"

GstDebugserverLog * gst_debugserver_log_new (void)
{
  GstDebugserverLog *log = (GstDebugserverLog*)g_malloc (sizeof(GstDebugserverLog));
  log->clients = NULL;

  return log;
}

void gst_debugserver_log_free (GstDebugserverLog * log)
{
  g_slist_free (log->clients);
  g_free (log);
}

void gst_debugserver_log_set_watch (GstDebugserverLog * log, gboolean enable,
  gpointer client_info)
{
  if (enable == TRUE) {
    if (g_slist_find (log->clients, client_info) == NULL) {
      log->clients = g_slist_append (log->clients, client_info);
    }
  } else {
    log->clients = g_slist_remove (log->clients, client_info);
  }
}

GSList* gst_debugserver_log_get_clients (GstDebugserverLog * log)
{
  return log->clients;
}

gint gst_debugserver_log_prepare_buffer (GstDebugMessage * gst_debug_msg,
  gchar * buffer, gint max_size)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  GstreamerLog log = GSTREAMER_LOG__INIT;
  gint size;

  log.level = 0; //todo
  log.message = g_strdup (gst_debug_message_get (gst_debug_msg));
  info.info_type = GSTREAMER_INFO__INFO_TYPE__LOG;
  info.log = &log;
  size = gstreamer_info__get_packed_size (&info);
  assert(size <= max_size);
  gstreamer_info__pack (&info, (guint8*)buffer);

  return size;
}
