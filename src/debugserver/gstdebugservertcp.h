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

#ifndef __GST_DEBUGSERVER_TCP_H__
#define __GST_DEBUGSERVER_TCP_H__

#include "protocol/gstdebugger.pb-c.h"

#include <gio/gio.h>

G_BEGIN_DECLS

#define GST_TYPE_DEBUGSERVER_TCP \
  (gst_debugserver_tcp_get_type())
#define GST_DEBUGSERVER_TCP(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_DEBUGSERVER_TCP,GstDebugserverTcp))
#define GST_TYPE_DEBUGSERVER_TCP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_DEBUGSERVER_TCP,GstDebugserverTcpClass))
#define GST_IS_DEBUGSERVER_TCP(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_DEBUGSERVER_TCP))
#define GST_IS_DEBUGSERVER_TCP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_DEBUGSERVER_TCP))
#define GST_DEBUGSERVER_TCP_CAST(obj) ((GstDebugserverTcp *)(obj))

typedef struct _GstDebugserverTcp GstDebugserverTcp;
typedef struct _GstDebugserverTcpClass GstDebugserverTcpClass;

typedef void (*GstDebugserverTcpProcessCommandFunction)
    (Command * command, gpointer client_id, gpointer user_data);

typedef void (*GstDebugserverTcpClientDisconnected)
    (gpointer client_id, gpointer user_data);

typedef struct _TcpClient {
  GSocketConnection * connection;
  GMutex mutex;
} TcpClient;

struct _GstDebugserverTcp {
  GObject parent_instance;

  GstDebugserverTcpProcessCommandFunction process_command;
  gpointer process_command_user_data;

  GstDebugserverTcpClientDisconnected client_disconnected;
  gpointer client_disconnected_user_data;

  /*< private >*/
  GSocketService * service;
  guint port;
  GSList * clients;
};

struct _GstDebugserverTcpClass
{
  GObjectClass parent_class;
};

GstDebugserverTcp * gst_debugserver_tcp_new (void);

void gst_debugserver_tcp_start_server (GstDebugserverTcp * tcp, guint port);

void gst_debugserver_tcp_stop_server (GstDebugserverTcp * tcp);

gboolean gst_debugserver_tcp_send_packet (GstDebugserverTcp * tcp, GSocketConnection * connection,
  gchar * buffer, gint size);

gboolean gst_debugserver_tcp_send_packet_to_all_clients (GstDebugserverTcp * tcp, gchar * buffer, gint size);

G_GNUC_INTERNAL GType gst_debugserver_tcp_get_type (void);

G_END_DECLS

#endif /* __GST_DEBUGSERVER_TCP_H__ */
