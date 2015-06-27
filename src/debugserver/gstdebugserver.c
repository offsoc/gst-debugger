/* GStreamer
 * Copyright (C) 2015 Marcin Kolny <marcin.kolny@gmail.com>
 *
 * gstdebugserver.c: tracing module that sends serialized data to
 * an user.
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
/**
 * SECTION:gstdebugserver
 * @short_description: communicate with user via tcp/ip protocol
 *
 * A tracing module that communicates with user via tcp/ip.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "gstdebugserver.h"

#include <string.h>

GST_DEBUG_CATEGORY_STATIC (gst_debugserver_debug);
#define GST_CAT_DEFAULT gst_debugserver_debug
#define _do_init \
    GST_DEBUG_CATEGORY_INIT (gst_debugserver_debug, "debugserver", 0, "debugserver tracer");
#define gst_debugserver_tracer_parent_class parent_class
G_DEFINE_TYPE_WITH_CODE (GstDebugserverTracer, gst_debugserver_tracer,
    GST_TYPE_TRACER, _do_init);

#define DEFAULT_PORT 8080

enum
{
  PROP_0,
  PROP_PORT
};

static void gst_debugserver_tracer_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_debugserver_tracer_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static void gst_debugserver_tracer_finalize (GObject * obj);

static void
gst_debugserver_tracer_log_function (GstDebugCategory * category,
    GstDebugLevel level, const gchar * file, const gchar * function, gint line,
    GObject * object, GstDebugMessage * message, gpointer user_data)
    G_GNUC_NO_INSTRUMENT;

static void gst_debugserver_tracer_close_connection (GstDebugserverTracer * debugserver)
{
  gst_debugserver_tcp_stop_server (debugserver->tcp_server);
  // todo free list
}

static void
message_broadcaster (GstBus * bus, GstMessage * message, gpointer user_data)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (user_data);
  GSocketConnection *connection;
  GList *clients = gst_debugserver_message_get_clients (debugserver->msg_handler,
    GST_MESSAGE_TYPE (message));
  gsize size;
  gchar buff[1024];

  while (clients != NULL) {
    connection = (GSocketConnection*)clients->data;
    size = gst_debugserver_message_prepare_buffer (message, buff, 1024);
    gst_debugserver_tcp_send_packet (g_socket_connection_get_socket (connection),
      buff, size);
    clients = clients->next;
  }
}

static void
do_element_new (GstTracer * self, guint64 ts, GstElement * element)
{
  GstBus *bus;
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (self);
  if (GST_IS_PIPELINE (element)) {
    debugserver->pipeline = GST_PIPELINE (element);
    bus = gst_pipeline_get_bus (debugserver->pipeline);
    gst_bus_add_signal_watch (bus);
    g_signal_connect (bus, "message", G_CALLBACK (message_broadcaster), debugserver);
  }
}

static void
do_push_event_pre (GstTracer * self, guint64 ts, GstPad * pad, GstEvent * event)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (self);
  GSocketConnection *connection;
  GSList *clients = gst_debugserver_event_get_clients (debugserver->event_handler);
  gsize size;
  gchar buff[1024];

  while (clients != NULL) {
    connection = (GSocketConnection*)clients->data;
    size = gst_debugserver_event_prepare_buffer (event, buff, 1024);
    gst_debugserver_tcp_send_packet (g_socket_connection_get_socket (connection),
      buff, size);
    clients = clients->next;
  }
}

static void
gst_debugserver_tracer_send_categories (GstDebugserverTracer * debugserver, gpointer client_id)
{
  gchar buffer[1024];
  gint size;
  GSocketConnection *connection = (GSocketConnection*) client_id;

  size = gst_debugserver_log_prepare_categories_buffer (buffer, 1024);
  gst_debugserver_tcp_send_packet (g_socket_connection_get_socket (connection),
    buffer, size);
}

static void
gst_debugserver_tracer_client_disconnected (gpointer client_id, gpointer user_data)
{
  GstDebugserverTracer *debugserver =GST_DEBUGSERVER_TRACER (user_data);

  gst_debugserver_log_set_watch (debugserver->log_handler, FALSE, client_id);
  gst_debugserver_event_set_watch (debugserver->event_handler, FALSE, client_id);
  //todo message
}

static void
gst_debugserver_tracer_process_command (Command * cmd, gpointer client_id,
  gpointer user_data)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (user_data);

  switch (cmd->command_type) {
  case COMMAND__COMMAND_TYPE__LOG_THRESHOLD:
    gst_debug_set_threshold_from_string (cmd->log_threshold->list, cmd->log_threshold->overwrite);
    break;
  case COMMAND__COMMAND_TYPE__MESSAGE_WATCH:
    gst_debugserver_message_set_watch (debugserver->msg_handler,
          cmd->message_watch->toggle == TOGGLE__ENABLE,
          cmd->message_watch->message_type, client_id);
    break;
  case COMMAND__COMMAND_TYPE__LOG_WATCH:
    gst_debugserver_log_set_watch (debugserver->log_handler,
          cmd->log_watch->toggle == TOGGLE__ENABLE, client_id);
    break;
  case COMMAND__COMMAND_TYPE__PAD_WATCH:
    gst_debugserver_event_set_watch (debugserver->event_handler,
          cmd->pad_watch->toggle == TOGGLE__ENABLE, client_id);
      break;
  case COMMAND__COMMAND_TYPE__DEBUG_CATEGORIES:
      gst_debugserver_tracer_send_categories (debugserver, client_id);
      break;
  default:
    GST_WARNING_OBJECT (debugserver, "Unsupported command type %d", cmd->command_type);
  }
}

static void
gst_debugserver_tracer_class_init (GstDebugserverTracerClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->set_property = gst_debugserver_tracer_set_property;
  gobject_class->get_property = gst_debugserver_tracer_get_property;
  gobject_class->finalize = gst_debugserver_tracer_finalize;

  g_object_class_install_property (gobject_class, PROP_PORT,
      g_param_spec_int ("port", "Port", "Server port",
          0, 65535, DEFAULT_PORT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gst_debugserver_tracer_log_function (GstDebugCategory * category,
    GstDebugLevel level, const gchar * file, const gchar * function, gint line,
    GObject * object, GstDebugMessage * message, gpointer user_data)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (user_data);
  GSocketConnection *connection;
  GSList *clients = gst_debugserver_log_get_clients (debugserver->log_handler);
  gsize size;
  gchar buff[1024];

  while (clients != NULL) {
    connection = (GSocketConnection*)clients->data;
    size = gst_debugserver_log_prepare_buffer (category, level, file, function,
      line, object, message, buff, 1024);
    gst_debugserver_tcp_send_packet (g_socket_connection_get_socket (connection),
      buff, size);
    clients = clients->next;
  }
}

static void
gst_debugserver_tracer_init (GstDebugserverTracer * self)
{
  GstTracer *tracer = GST_TRACER (self);

  self->pipeline = NULL;
  self->port = DEFAULT_PORT;
  self->msg_handler = gst_debugserver_message_new ();
  self->log_handler = gst_debugserver_log_new ();
  self->event_handler = gst_debugserver_event_new ();

  gst_tracing_register_hook (tracer, "element-new",
      G_CALLBACK (do_element_new));
  gst_tracing_register_hook (tracer, "pad-push-event-pre",
      G_CALLBACK (do_push_event_pre));

  gst_debug_add_log_function (gst_debugserver_tracer_log_function, self, NULL);

  self->tcp_server = gst_debugserver_tcp_new ();
  self->tcp_server->process_command = gst_debugserver_tracer_process_command;
  self->tcp_server->process_command_user_data = self;
  self->tcp_server->client_disconnected = gst_debugserver_tracer_client_disconnected;
  self->tcp_server->client_disconnected_user_data = self;
  gst_debugserver_tcp_start_server (self->tcp_server, self->port);
}

static void
gst_debugserver_tracer_finalize (GObject * obj)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (obj);

  gst_debug_remove_log_function (gst_debugserver_tracer_log_function);

  gst_debugserver_message_free (debugserver->msg_handler);
  gst_debugserver_log_free (debugserver->log_handler);
  gst_debugserver_event_free (debugserver->event_handler);
  gst_debugserver_tracer_close_connection (debugserver);
  g_object_unref (G_OBJECT (debugserver->tcp_server));
}

static void
gst_debugserver_tracer_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstDebugserverTracer *debugserver;
  gint tmp_port;

  debugserver = GST_DEBUGSERVER_TRACER (object);

  switch (prop_id) {
    case PROP_PORT:
      tmp_port = g_value_get_int (value);
      if (tmp_port != debugserver->port) {
        debugserver->port = g_value_get_int (value);
        gst_debugserver_tracer_close_connection (debugserver);
        gst_debugserver_tcp_start_server (debugserver->tcp_server, debugserver->port);
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_debugserver_tracer_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstDebugserverTracer *debugserver;

  debugserver = GST_DEBUGSERVER_TRACER (object);

  switch (prop_id) {
    case PROP_PORT:
      g_value_set_int (value, debugserver->port);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_tracer_register (plugin, "debugserver",
          gst_debugserver_tracer_get_type ()))
    return FALSE;
  return TRUE;
}

#define PACKAGE "gst-debugger"

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR, debugtracers,
    "GStreamer core tracers", plugin_init, "1.0", "LGPL",
    "gst-debugger", "http://github.com/loganek");
