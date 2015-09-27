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

#ifndef __GST_DEBUGSERVER_MESSAGE_H__
#define __GST_DEBUGSERVER_MESSAGE_H__

#include "gstdebugservertcp.h"
#include "gstdebugserverwatcher.h"

#include <gst/gst.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _GstDebugserverMessage GstDebugserverMessage;

typedef struct {
  GHashTable *clients;

  gboolean (*accept_client) (gpointer);
  void (*serialize_data) (GstDebugger__GStreamerData*);
  void (*free_data) (GstDebugger__GStreamerData*);
  void (*compare) (gpointer, gpointer);
} Watcher;


struct _GstDebugserverMessage {
  GstDebugserverWatcher watcher;
};

GstDebugserverMessage * gst_debugserver_message_new (void);

void gst_debugserver_message_free (GstDebugserverMessage * msg);

void gst_debugserver_message_clean (GstDebugserverMessage * msg);

gboolean gst_debugserver_message_set_watch (GstDebugserverMessage * msg,
  TcpClient * client, GstDebugger__MessageRequest * request);

void gst_debugserver_message_remove_client (GstDebugserverMessage * msg,
  TcpClient * client);

void gst_debugserver_message_send_message (GstDebugserverMessage * msg,
  GstDebugserverTcp * tcp_server, GstMessage * gst_msg);

G_END_DECLS

#endif /* __GST_DEBUGSERVER_MESSAGE_H__ */
