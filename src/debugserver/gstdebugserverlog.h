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

#ifndef __GST_DEBUGSERVER_LOG_H__
#define __GST_DEBUGSERVER_LOG_H__

#include "gstdebugserverwatcher.h"

#include <gst/gst.h>

G_BEGIN_DECLS

typedef struct _GstDebugserverLog GstDebugserverLog;

struct _GstDebugserverLog {
  GstDebugserverWatcher watcher;
};

GstDebugserverLog * gst_debugserver_log_new (void);

void gst_debugserver_log_free (GstDebugserverLog * log);

void gst_debugserver_log_send_log (GstDebugserverLog * log, GstDebugserverTcp * tcp_server,
  GstDebugCategory * category, GstDebugLevel level, const gchar * file, const gchar * function,
  gint line, GObject * object, GstDebugMessage * message);

gboolean gst_debugserver_log_set_watch (GstDebugserverLog * log, gboolean enable, gint level,
  const gchar * category, TcpClient * client);

void gst_debugserver_log_send_debug_categories (GstDebugserverTcp *tcp_server, TcpClient *client);

void gst_debugserver_log_set_threshold (const gchar * threshold);

void gst_debugserver_log_clean (GstDebugserverLog * log);

void gst_debugserver_log_remove_client (GstDebugserverLog * log,
  TcpClient * client);

G_END_DECLS

#endif /* __GST_DEBUGSERVER_LOG_H__ */
