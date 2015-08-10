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
#include "gstdebugservertopology.h"
#include "utils/gst-utils.h"
#include "utils/buffer-prepare-utils.h"
#include "protocol/serializer.h"
#include "protocol/deserializer.h"

#include <string.h>

GST_DEBUG_CATEGORY_STATIC (gst_debugserver_debug);
#define GST_CAT_DEFAULT gst_debugserver_debug
#define _do_init \
    GST_DEBUG_CATEGORY_INIT (gst_debugserver_debug, "debugserver", 0, "debugserver tracer");
#define gst_debugserver_tracer_parent_class parent_class
G_DEFINE_TYPE_WITH_CODE (GstDebugserverTracer, gst_debugserver_tracer,
    GST_TYPE_TRACER, _do_init)

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
gst_debugserver_handle_error (GstDebugserverTracer *server,
  GSocketConnection * client_id, const gchar * message);

static void
gst_debugserver_tracer_log_function (GstDebugCategory * category,
    GstDebugLevel level, const gchar * file, const gchar * function, gint line,
    GObject * object, GstDebugMessage * message, gpointer user_data)
    G_GNUC_NO_INSTRUMENT;

static void gst_debugserver_tracer_close_connection (GstDebugserverTracer * debugserver)
{
  gst_debugserver_message_clean (debugserver->msg_handler);
  gst_debugserver_log_clean (debugserver->log_handler);
  gst_debugserver_qe_clean (debugserver->event_handler);
  gst_debugserver_qe_clean (debugserver->query_handler);
  gst_debugserver_buffer_clean (debugserver->buffer_handler);

  gst_debugserver_tcp_stop_server (debugserver->tcp_server);
}

static void
message_broadcaster (GstBus * bus, GstMessage * message, gpointer user_data)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (user_data);
  GSocketConnection *connection;
  GSList *clients = gst_debugserver_message_get_clients (debugserver->msg_handler,
    GST_MESSAGE_TYPE (message));
  GSList *cl_tmp = clients;
  gsize size;
  gchar buff[1024];

  while (clients != NULL) {
    connection = (GSocketConnection*)clients->data;
    size = gst_debugserver_qebm_prepare_buffer (GST_MINI_OBJECT (message), buff, 1024);
    gst_debugserver_tcp_send_packet (debugserver->tcp_server, connection,
      buff, size);
    clients = clients->next;
  }

  g_slist_free (cl_tmp);
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
do_pad_unlink_post (GstTracer * self, guint64 ts, GstPad * src, GstPad * sink, gboolean result)
{
  if (result == FALSE) {
    return;
  }
  gst_debugserver_topology_send_pad_link (src, sink, FALSE, GST_DEBUGSERVER_TRACER (self)->tcp_server, NULL);
}

static void
do_pad_link_post (GstTracer * self, guint64 ts, GstPad * src, GstPad * sink, GstPadLinkReturn result)
{
  if (result == FALSE) {
    return;
  }
  gst_debugserver_topology_send_pad_link (src, sink, TRUE, GST_DEBUGSERVER_TRACER (self)->tcp_server, NULL);
}

static void
do_bin_add_post (GstTracer * self, gint64 ts, GstBin * bin, GstElement * element, gboolean result)
{
  if (result == FALSE) {
    return;
  }
  gst_debugserver_topology_send_element_in_bin (bin, element, FALSE, GST_DEBUGSERVER_TRACER (self)->tcp_server, NULL);
}

static void
do_bin_remove_pre (GstTracer * self, gint64 ts, GstBin * bin, GstElement * element)
{
  gst_debugserver_topology_send_element_in_bin (bin, element, TRUE, GST_DEBUGSERVER_TRACER (self)->tcp_server, NULL);
}

static void
do_element_add_pad (GstTracer * self, gint64 ts, GstElement * element, GstPad * pad)
{
  gst_debugserver_topology_send_pad_in_element (element, pad, TRUE, GST_DEBUGSERVER_TRACER (self)->tcp_server, NULL);
}

static void
do_element_remove_pad (GstTracer * self, gint64 ts, GstElement * element, GstPad * pad)
{
  gst_debugserver_topology_send_pad_in_element (element, pad, FALSE, GST_DEBUGSERVER_TRACER (self)->tcp_server, NULL);
}


static void
do_push_event_pre (GstTracer * self, guint64 ts, GstPad * pad, GstEvent * event)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (self);
  GSocketConnection *connection;
  GSList *clients = gst_debugserver_qe_get_clients (debugserver->event_handler,
    pad, event->type);
  gsize size;
  SAFE_PREPARE_BUFFER_INIT (1024);

  if (clients == NULL) {
    return;
  }

  SAFE_PREPARE_BUFFER (
        gst_debugserver_qebm_prepare_buffer (GST_MINI_OBJECT (event), m_buff, max_m_buff_size), size);

  while (clients != NULL) {
    connection = (GSocketConnection*)clients->data;
    gst_debugserver_tcp_send_packet (GST_DEBUGSERVER_TRACER (self)->tcp_server, connection,
      m_buff, size);
    clients = clients->next;
  }

  SAFE_PREPARE_BUFFER_CLEAN;
}

static void
do_pad_query_pre (GstTracer * self, guint64 ts, GstPad * pad, GstQuery * query)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (self);
  GSocketConnection *connection;
  GSList *clients = gst_debugserver_qe_get_clients (debugserver->query_handler,
    pad, query->type);
  gsize size;
  gchar buff[1024];

  while (clients != NULL) {
    connection = (GSocketConnection*)clients->data;
    size = gst_debugserver_qebm_prepare_buffer (GST_MINI_OBJECT (query), buff, 1024);
    gst_debugserver_tcp_send_packet (GST_DEBUGSERVER_TRACER (self)->tcp_server, connection,
      buff, size);
    clients = clients->next;
  }
}

static void
do_pad_push_pre (GstTracer * self, guint64 ts, GstPad * pad, GstBuffer * buffer)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (self);
  GSocketConnection *connection;
  GSList *clients = gst_debugserver_buffer_get_clients (debugserver->buffer_handler,
    pad);
  gsize size;
  SAFE_PREPARE_BUFFER_INIT (1024);

  if (clients == NULL) {
    return;
  }

  SAFE_PREPARE_BUFFER (
    gst_debugserver_qebm_prepare_buffer (GST_MINI_OBJECT (buffer), m_buff, max_m_buff_size), size);

  while (clients != NULL) {
    connection = (GSocketConnection*)clients->data;
    gst_debugserver_tcp_send_packet (GST_DEBUGSERVER_TRACER (self)->tcp_server, connection,
      m_buff, size);
    clients = clients->next;
  }

  SAFE_PREPARE_BUFFER_CLEAN;

  g_slist_free (clients);
}

static gint
gst_debug_server_prepare_enum_type_buffer (GEnumClass * klass, gchar * buffer, gint size)
{
  GEnumValue *values = klass->values;
  guint i = 0;
  EnumEntry **entries;
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  EnumType msg = ENUM_TYPE__INIT;
  info.info_type = GSTREAMER_INFO__INFO_TYPE__ENUM_TYPE;
  gint len;

  entries = g_malloc (sizeof (EnumEntry) * (klass->n_values));

  for (i = 0; i < klass->n_values; i++) {
    entries[i] = g_malloc (sizeof (EnumEntry));
    enum_entry__init (entries[i]);
    entries[i]->name = g_strdup (values[i].value_name);
    entries[i]->value = values[i].value;
    entries[i]->nick = g_strdup (values[i].value_nick);
  }

  msg.entry = entries;
  msg.n_entry = klass->n_values;
  msg.type_name = g_strdup (G_ENUM_CLASS_TYPE_NAME (klass));
  info.enum_type = &msg;
  len = gstreamer_info__get_packed_size (&info);

  if (len > size) {
    goto finalize;
  }

  gstreamer_info__pack (&info, (uint8_t*) buffer);

finalize:
  for (i = 0; i < klass->n_values; i++) {
    g_free (entries[i]);
  }

  g_free (entries);

  return len;
}

// todo maybe I don't have to copy&paste enum method?
static gint
gst_debug_server_prepare_flags_type_buffer (GFlagsClass * klass, gchar * buffer, gint size)
{
  GFlagsValue *values = klass->values;
  guint i = 0;
  EnumEntry **entries;
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  EnumType msg = ENUM_TYPE__INIT;
  info.info_type = GSTREAMER_INFO__INFO_TYPE__ENUM_TYPE;
  gint len;

  entries = g_malloc (sizeof (EnumEntry) * (klass->n_values));

  for (i = 0; i < klass->n_values; i++) {
    entries[i] = g_malloc (sizeof (EnumEntry));
    enum_entry__init (entries[i]);
    entries[i]->name = g_strdup (values[i].value_name);
    entries[i]->value = values[i].value;
    entries[i]->nick = g_strdup (values[i].value_nick);
  }

  msg.entry = entries;
  msg.n_entry = klass->n_values;
  msg.type_name = g_strdup (G_FLAGS_CLASS_TYPE_NAME (klass));
  info.enum_type = &msg;
  len = gstreamer_info__get_packed_size (&info);

  if (len > size) {
    goto finalize;
  }

  gstreamer_info__pack (&info, (uint8_t*) buffer);

finalize:
  for (i = 0; i < klass->n_values; i++) {
    g_free (entries[i]);
  }

  g_free (entries);

  return len;
}

static void
gst_debugserver_send_enum (GstDebugserverTracer * debugserver, GSocketConnection * client, const gchar * klass_name)
{
  GType type = g_type_from_name (klass_name);
  GEnumClass *klass = g_type_class_peek (type);
  SAFE_PREPARE_BUFFER_INIT (1024);
  gint size;

  if (klass == NULL) {
    gst_debugserver_handle_error (debugserver, client, "Cannot find enum type");
    return;
  }

  SAFE_PREPARE_BUFFER (
    gst_debug_server_prepare_enum_type_buffer (klass, m_buff, max_m_buff_size), size);

  gst_debugserver_tcp_send_packet (debugserver->tcp_server, client, m_buff, size);
  SAFE_PREPARE_BUFFER_CLEAN;
}

static void
gst_debugserver_send_flag (GstDebugserverTracer * debugserver, GSocketConnection * client, const gchar * klass_name)
{
  GType type = g_type_from_name (klass_name);
  GFlagsClass *klass = g_type_class_peek (type);
  SAFE_PREPARE_BUFFER_INIT (1024);
  gint size;

  if (klass == NULL) {
    gst_debugserver_handle_error (debugserver, client, "Cannot find flags type");
    return;
  }

  SAFE_PREPARE_BUFFER (
    gst_debug_server_prepare_flags_type_buffer (klass, m_buff, max_m_buff_size), size);

  gst_debugserver_tcp_send_packet (debugserver->tcp_server, client, m_buff, size);
  SAFE_PREPARE_BUFFER_CLEAN;
}

static void
gst_debugserver_send_enum_flags (GstDebugserverTracer * debugserver, GSocketConnection * client, const gchar * klass_name) {
  GType type = g_type_from_name (klass_name);

  if (G_TYPE_IS_ENUM (type)) {
    gst_debugserver_send_enum (debugserver, client, klass_name);
  } else if (G_TYPE_IS_FLAGS (type)) {
    gst_debugserver_send_flag (debugserver, client, klass_name);
  } else {
    gst_debugserver_handle_error (debugserver, client, "requested type info is neither enum nor flags");
  }
}

static void
gst_debugserver_tracer_send_categories (GstDebugserverTracer * debugserver, gpointer client_id)
{
  gint size;
  GSocketConnection *connection = (GSocketConnection*) client_id;
  SAFE_PREPARE_BUFFER_INIT (1024);
  SAFE_PREPARE_BUFFER (gst_debugserver_log_prepare_categories_buffer (m_buff, max_m_buff_size), size);
  gst_debugserver_tcp_send_packet (debugserver->tcp_server, connection,
    m_buff, size);
  SAFE_PREPARE_BUFFER_CLEAN;
}

static void
gst_debugserver_tracer_client_disconnected (gpointer client_id, gpointer user_data)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (user_data);

  gst_debugserver_log_set_watch (debugserver->log_handler, FALSE, client_id);
  // todo this methods remove only one watch
  gst_debugserver_qe_set_watch (debugserver->event_handler, FALSE, NULL,
    -1, client_id);
  gst_debugserver_qe_set_watch (debugserver->query_handler, FALSE, NULL,
    -1, client_id);
  gst_debugserver_buffer_set_watch (debugserver->buffer_handler, FALSE, NULL,
    client_id);
  gst_debugserver_message_remove_client (debugserver->msg_handler, client_id);
}

static void
gst_debugserver_tracer_client_connected (gpointer client_id, gpointer user_data)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (user_data);

  gst_debugserver_topology_send_entire_topology (
    GST_BIN (debugserver->pipeline), debugserver->tcp_server, client_id);

  gst_debugserver_send_enum (debugserver, client_id, "GstQueryType");

  gst_debugserver_send_enum (debugserver, client_id, "GstEventType");

  gst_debugserver_send_flag (debugserver, client_id, "GstMessageType");
}

static gint
gst_debugserver_prepare_property (const gchar * element_path, const GstElement *element, const GParamSpec *param, gchar * buffer, gint max_size)
{
  GValue value = {0};
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  Property property = PROPERTY__INIT;
  info.info_type = GSTREAMER_INFO__INFO_TYPE__PROPERTY;
  gint size;
  GType tmptype;
  InternalGType internal_type;

  g_value_init (&value, param->value_type);
  g_object_get_property (G_OBJECT (element), param->name, &value);

  property.element_path = g_strdup (element_path);
  property.property_name = g_strdup (param->name);
  property.property_value = g_value_serialize (&value, &tmptype, &internal_type);
  property.internal_type = internal_type;
  property.has_internal_type = TRUE;
  property.type = tmptype;
  property.has_type = TRUE;
  property.type_name = g_strdup (g_type_name (value.g_type));
  property.description = g_strdup (g_param_spec_get_blurb ((GParamSpec*) param));
  info.property = &property;
  size = gstreamer_info__get_packed_size (&info);

  if (size > max_size) {
    goto finalize;
   }
  gstreamer_info__pack (&info, (guint8*)buffer);

finalize:
  return size;
}

static void
gst_debugserver_property_send_single_property (GstDebugserverTracer * server, GSocketConnection * client_id,
  const gchar * element_path, const GstElement *element, const GParamSpec *param)
{
  gint size;
  SAFE_PREPARE_BUFFER_INIT (1024);

  SAFE_PREPARE_BUFFER (
    gst_debugserver_prepare_property (element_path, element, param, m_buff, max_m_buff_size), size);

  gst_debugserver_tcp_send_packet (server->tcp_server, client_id, m_buff, size);

  SAFE_PREPARE_BUFFER_CLEAN;
}

static gint
gst_debugserver_prepare_error (const gchar * message, gchar * buffer, gint max_size)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  ServerError s_err = SERVER_ERROR__INIT;
  gint size;

  info.info_type = GSTREAMER_INFO__INFO_TYPE__SERVER_ERROR;
  s_err.message = g_strdup (message);
  info.server_error = &s_err;

  size = gstreamer_info__get_packed_size (&info);

  if (size > max_size) {
    goto finalize;
  }

  gstreamer_info__pack (&info, (guint8*)buffer);

finalize:
  return size;
}

static void
gst_debugserver_handle_error (GstDebugserverTracer *server, GSocketConnection * client_id, const gchar * message)
{
  gint size;
  SAFE_PREPARE_BUFFER_INIT (1024);

  GST_WARNING_OBJECT (server, "%s", message);

  SAFE_PREPARE_BUFFER (
    gst_debugserver_prepare_error (message, m_buff, max_m_buff_size), size);

  gst_debugserver_tcp_send_packet (server->tcp_server, client_id, m_buff, size);

  SAFE_PREPARE_BUFFER_CLEAN;
}

static void
gst_debugserver_property_send_property (GstDebugserverTracer * server, GSocketConnection * client_id, const gchar * element_path, const gchar * property_name)
{
  GstElement *element = gst_utils_get_element_from_path (GST_ELEMENT_CAST (server->pipeline), element_path);

  if (element == NULL) {
    gst_debugserver_handle_error (server, client_id, "Cannot find element");
    return;
  }

  GstElementClass *element_class = GST_ELEMENT_GET_CLASS (element);

  if (property_name == NULL || strlen (property_name) == 0) {
    guint num_properties, i;
    GParamSpec **property_specs = g_object_class_list_properties
        (G_OBJECT_GET_CLASS (element), &num_properties);

    for (i = 0; i < num_properties; i++) {
      gst_debugserver_property_send_single_property (server, client_id, element_path, element, property_specs[i]);
    }

    g_free (property_specs);
  } else {
    GParamSpec *param = g_object_class_find_property ((GObjectClass *)element_class, property_name);
    if (param == NULL) {
      gst_debugserver_handle_error (server, client_id, "Cannot find property");
      return;
    }
    gst_debugserver_property_send_single_property (server, client_id, element_path, element, param);
  }
}

static void
gst_debugserver_tracer_process_command (Command * cmd, gpointer client_id,
  gpointer user_data)
{
  gchar buff[1024];
  gint size;
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (user_data);

  switch (cmd->command_type) {
  case COMMAND__COMMAND_TYPE__LOG_THRESHOLD:
    gst_debug_set_threshold_from_string (cmd->log_threshold->list, cmd->log_threshold->overwrite);
    break;
  case COMMAND__COMMAND_TYPE__MESSAGE_WATCH:
    if (gst_debugserver_message_set_watch (debugserver->msg_handler,
          cmd->message_watch->toggle == TOGGLE__ENABLE,
          cmd->message_watch->message_type, client_id)) {
      size = gst_debugserver_message_prepare_confirmation_buffer (cmd->message_watch,
        buff, 1024);
      gst_debugserver_tcp_send_packet (debugserver->tcp_server, client_id,
        buff, size);
    }
    break;
  case COMMAND__COMMAND_TYPE__LOG_WATCH:
    gst_debugserver_log_set_watch (debugserver->log_handler,
          cmd->log_watch->toggle == TOGGLE__ENABLE, client_id);
    break;
  case COMMAND__COMMAND_TYPE__PAD_WATCH:
    // todo refactor - too complicated
    if (cmd->pad_watch->watch_type == PAD_WATCH__WATCH_TYPE__EVENT) {
      if (gst_debugserver_qe_set_watch (debugserver->event_handler,
            cmd->pad_watch->toggle == TOGGLE__ENABLE,
			gst_utils_get_pad_from_path (GST_ELEMENT (debugserver->pipeline), cmd->pad_watch->pad_path),
            cmd->pad_watch->qe_type, client_id)) {
        size = gst_debugserver_qeb_prepare_confirmation_buffer (cmd->pad_watch->pad_path,
          cmd->pad_watch->qe_type, cmd->pad_watch->toggle, buff, 1024, PAD_WATCH__WATCH_TYPE__EVENT);
        gst_debugserver_tcp_send_packet (debugserver->tcp_server, client_id,
          buff, size);
      }
    } else if (cmd->pad_watch->watch_type == PAD_WATCH__WATCH_TYPE__QUERY) {
        if (gst_debugserver_qe_set_watch (debugserver->query_handler,
              cmd->pad_watch->toggle == TOGGLE__ENABLE,
              gst_utils_get_pad_from_path (GST_ELEMENT (debugserver->pipeline), cmd->pad_watch->pad_path),
              cmd->pad_watch->qe_type, client_id)) {
          size = gst_debugserver_qeb_prepare_confirmation_buffer (cmd->pad_watch->pad_path,
            cmd->pad_watch->qe_type, cmd->pad_watch->toggle, buff, 1024, PAD_WATCH__WATCH_TYPE__QUERY);
          gst_debugserver_tcp_send_packet (debugserver->tcp_server, client_id,
            buff, size);
        }
      } else if (cmd->pad_watch->watch_type == PAD_WATCH__WATCH_TYPE__BUFFER) {
        if (gst_debugserver_buffer_set_watch (debugserver->buffer_handler,
              cmd->pad_watch->toggle == TOGGLE__ENABLE,
              gst_utils_get_pad_from_path (GST_ELEMENT (debugserver->pipeline), cmd->pad_watch->pad_path),
              client_id)) {
          size = gst_debugserver_qeb_prepare_confirmation_buffer (cmd->pad_watch->pad_path,
            -1, cmd->pad_watch->toggle, buff, 1024, PAD_WATCH__WATCH_TYPE__BUFFER);
          gst_debugserver_tcp_send_packet (debugserver->tcp_server, client_id,
            buff, size);
        }
      }
    break;
  case COMMAND__COMMAND_TYPE__DEBUG_CATEGORIES:
      gst_debugserver_tracer_send_categories (debugserver, client_id);
      break;
  case COMMAND__COMMAND_TYPE__TOPOLOGY:
    gst_debugserver_topology_send_entire_topology (GST_BIN (debugserver->pipeline), debugserver->tcp_server, client_id);
    break;
  case COMMAND__COMMAND_TYPE__PROPERTY:
    if (cmd->property->has_type == FALSE) {
      gst_debugserver_property_send_property (debugserver, client_id, cmd->property->element_path, cmd->property->property_name);
    } else {
      GValue val = G_VALUE_INIT;
      GstElement *element = gst_utils_get_element_from_path (GST_ELEMENT_CAST (debugserver->pipeline), cmd->property->element_path);
      g_value_deserialize (&val, cmd->property->type, cmd->property->internal_type, cmd->property->property_value);
      g_object_set_property (G_OBJECT (element), cmd->property->property_name, &val);
      g_value_unset (&val);
    }
    break;
  case COMMAND__COMMAND_TYPE__ENUM_TYPE:
    gst_debugserver_send_enum_flags (debugserver, client_id, cmd->enum_name);
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
  SAFE_PREPARE_BUFFER_INIT (1024);

  if (clients == NULL) {
    return;
  }

  SAFE_PREPARE_BUFFER (gst_debugserver_log_prepare_buffer (category, level, file, function,
    line, object, message, m_buff, max_m_buff_size), size);

  while (clients != NULL) {
    connection = (GSocketConnection*)clients->data;
    gst_debugserver_tcp_send_packet (debugserver->tcp_server, connection,
      m_buff, size);
    clients = clients->next;
  }

  SAFE_PREPARE_BUFFER_CLEAN;
}

static void
gst_debugserver_tracer_init (GstDebugserverTracer * self)
{
  GstTracer *tracer = GST_TRACER (self);

  self->pipeline = NULL;
  self->port = DEFAULT_PORT;
  self->msg_handler = gst_debugserver_message_new ();
  self->log_handler = gst_debugserver_log_new ();
  self->event_handler = gst_debugserver_qe_new ();
  self->query_handler = gst_debugserver_qe_new ();
  self->buffer_handler = gst_debugserver_buffer_new ();

  gst_tracing_register_hook (tracer, "element-new",
      G_CALLBACK (do_element_new));
  gst_tracing_register_hook (tracer, "pad-push-event-pre",
      G_CALLBACK (do_push_event_pre));
  gst_tracing_register_hook (tracer, "pad-query-pre",
      G_CALLBACK (do_pad_query_pre));
  gst_tracing_register_hook (tracer, "pad-push-pre",
      G_CALLBACK (do_pad_push_pre));
  gst_tracing_register_hook (tracer, "element-add-pad",
      G_CALLBACK (do_element_add_pad));
  gst_tracing_register_hook (tracer, "element-remove-pad",
      G_CALLBACK (do_element_remove_pad));
  gst_tracing_register_hook (tracer, "bin-add-post",
      G_CALLBACK (do_bin_add_post));
  gst_tracing_register_hook (tracer, "bin-remove-pre",
      G_CALLBACK (do_bin_remove_pre));
  gst_tracing_register_hook (tracer, "pad-link-post",
      G_CALLBACK (do_pad_link_post));
  gst_tracing_register_hook (tracer, "pad-unlink-post",
      G_CALLBACK (do_pad_unlink_post));

  gst_debug_add_log_function (gst_debugserver_tracer_log_function, self, NULL);

  self->tcp_server = gst_debugserver_tcp_new ();
  self->tcp_server->process_command = gst_debugserver_tracer_process_command;
  self->tcp_server->process_command_user_data = self;
  self->tcp_server->client_disconnected = gst_debugserver_tracer_client_disconnected;
  self->tcp_server->parent = self;
  self->tcp_server->client_connected = gst_debugserver_tracer_client_connected;
  gst_debugserver_tcp_start_server (self->tcp_server, self->port);
}

static void
gst_debugserver_tracer_finalize (GObject * obj)
{
  GstDebugserverTracer *debugserver = GST_DEBUGSERVER_TRACER (obj);

  gst_debug_remove_log_function (gst_debugserver_tracer_log_function);

  gst_debugserver_tracer_close_connection (debugserver);

  gst_debugserver_message_free (debugserver->msg_handler);
  gst_debugserver_log_free (debugserver->log_handler);
  gst_debugserver_qe_free (debugserver->event_handler);
  gst_debugserver_qe_free (debugserver->query_handler);
 // gst_debugserver_buffer_free (debugserver->buffer_handler);

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
    "gst-debugger", "http://github.com/loganek")
