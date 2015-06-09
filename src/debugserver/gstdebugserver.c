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
static void gst_debugserver_tracer_start_server (GstDebugserverTracer *
    debugserver);
static void gst_debugserver_tracer_stop_server (GstDebugserverTracer *
    debugserver);
static void gst_debugserver_tracer_finalize (GObject * obj);

gboolean incoming_callback (GSocketService * service,
    GSocketConnection * connection,
    GObject * source_object, gpointer user_data);


static void
do_element_new (GstTracer * self, guint64 ts, GstElement * element)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (self);
  if (GST_IS_PIPELINE (element)) {
    debugserver->pipeline = GST_PIPELINE (element);
  }
}

static void
do_push_buffer_pre (GstTracer * self, guint64 ts, GstPad * pad,
    GstBuffer * buffer)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (self);
  GOutputStream *ostream = NULL;
  gsize bytes_written;
  GList *listeners;

  listeners =
      g_hash_table_lookup (debugserver->pre_push_listeners,
      gst_pad_get_name (pad));
  while (listeners != NULL) {
    ostream = g_io_stream_get_output_stream (G_IO_STREAM (listeners->data));

    g_output_stream_printf (ostream, &bytes_written, NULL,
        NULL, "%lu\n", GST_BUFFER_PTS (buffer));

    listeners = g_list_next (listeners);
  }
}

// todo key should be GstPad* type
static void
gst_debugserver_tracer_start_watch (GstDebugserverTracer * debugserver,
    GSocketConnection * connection, const gchar * key)
{
  GList *listeners =
      (GList *) g_hash_table_lookup (debugserver->pre_push_listeners, key);

  if (listeners == NULL) {
    listeners = g_list_append (listeners, connection);
    g_hash_table_insert (debugserver->pre_push_listeners, g_strdup (key),
        listeners);
    return;
  }

  if (g_list_find (listeners, connection) == NULL) {
    listeners = g_list_append (listeners, connection);
    g_hash_table_replace (debugserver->pre_push_listeners, g_strdup (key),
        listeners);
  }
}

// todo key should be GstPad* type
static void
gst_debugserver_tracer_stop_watch (GstDebugserverTracer * debugserver,
    GSocketConnection * connection, const gchar * key)
{
  GList *listeners =
      (GList *) g_hash_table_lookup (debugserver->pre_push_listeners, key);

  listeners = g_list_remove (listeners, connection);
  g_hash_table_replace (debugserver->pre_push_listeners, g_strdup (key),
      listeners);
}

static gpointer
gst_debugserver_tracer_process_client (gpointer user_data)
{
  GArray *tmp = (GArray *) user_data;
  GSocketConnection *connection = g_array_index (tmp, GSocketConnection *, 0);
  GstDebugserverTracer *debugserver =
      g_array_index (tmp, GstDebugserverTracer *, 1);
  GInputStream *istream;
  GHashTableIter iter;
  gpointer key, value;
  gchar message[1024];

  g_array_unref (tmp);

  istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));

  GST_DEBUG_OBJECT (debugserver, "Received connection from client!\n");

  while (g_input_stream_read (istream, message, 1024, NULL, NULL)) {
    g_print ("Message was: \"%s\"\n", message);

    if (strncmp (message, "start", 5) == 0) {
      gst_debugserver_tracer_start_watch (debugserver, connection, "src");
    } else if (strncmp (message, "stop", 4) == 0) {
      gst_debugserver_tracer_stop_watch (debugserver, connection, "src");
    }
  }

  g_hash_table_iter_init (&iter, debugserver->pre_push_listeners);
  while (g_hash_table_iter_next (&iter, &key, &value)) {
    gst_debugserver_tracer_stop_watch (debugserver, connection, key);
  }

  return NULL;
}

gboolean
incoming_callback (GSocketService * service,
    GSocketConnection * connection, GObject * source_object, gpointer user_data)
{
  GArray *tmp = g_array_new (FALSE, FALSE, sizeof (gpointer));
  g_array_insert_val (tmp, 0, connection);
  g_array_insert_val (tmp, 1, user_data);
  g_object_ref (connection);
  g_thread_new ("connection",
      (GThreadFunc) gst_debugserver_tracer_process_client, tmp);
  return TRUE;
}

/* tracer class */

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
    //G_GNUC_NO_INSTRUMENT todo
{

}

static void
gst_debugserver_tracer_init (GstDebugserverTracer * self)
{
  GstTracer *tracer = GST_TRACER (self);
  g_print("start server");
  self->pipeline = NULL;
  self->port = DEFAULT_PORT;
  self->pre_push_listeners = g_hash_table_new_full (g_str_hash,
      g_str_equal, g_free, NULL);
  gst_debugserver_tracer_start_server (self);

  gst_tracing_register_hook (tracer, "pad-push-pre",
      G_CALLBACK (do_push_buffer_pre));

  gst_tracing_register_hook (tracer, "element-new",
      G_CALLBACK (do_element_new));

  gst_debug_add_log_function (gst_debugserver_tracer_log_function, self, NULL);
}

static void
gst_debugserver_tracer_finalize (GObject * obj)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (obj);

  gst_debugserver_tracer_stop_server (debugserver);
  g_hash_table_unref (debugserver->pre_push_listeners);
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
        gst_debugserver_tracer_stop_server (debugserver);
        gst_debugserver_tracer_start_server (debugserver);
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

static void
gst_debugserver_tracer_start_server (GstDebugserverTracer * debugserver)
{
  GError *error = NULL;
  debugserver->service = g_socket_service_new ();

  g_socket_listener_add_inet_port ((GSocketListener *) debugserver->service,
      debugserver->port, NULL, &error);

  if (error != NULL) {
    GST_ERROR_OBJECT (debugserver, "%s", error->message);
    g_error_free (error);
  }

  g_signal_connect (debugserver->service,
      "incoming", G_CALLBACK (incoming_callback), debugserver);

  g_socket_service_start (debugserver->service);
}

static void
gst_debugserver_tracer_stop_server (GstDebugserverTracer * debugserver)
{
  if (debugserver->service != NULL) {
    g_socket_service_stop (debugserver->service);
    g_hash_table_remove_all (debugserver->pre_push_listeners);
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
