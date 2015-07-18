/*
 * serializer.c
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "serializer.h"
#include "protocol_utils.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

gint gst_query_serialize (GstQuery * query, gchar * buffer, gint size)
{
  gchar * str;
  gint str_len;
  gint type_size;
  gint total_size;
  const GstStructure * q_structure = gst_query_get_structure (query);

  str = gst_structure_to_string (q_structure);
  str_len = strlen (str) + 1;
  type_size = 4;
  total_size = type_size + str_len;
  if (total_size > size) {
    goto finalize;
  }

  gst_debugger_protocol_utils_serialize_integer64 (query->type, buffer, type_size);
  memcpy (buffer + type_size, str, str_len);

finalize:
  g_free (str);
  return total_size;
}

gint gst_event_serialize (GstEvent * event, gchar * buffer, gint size)
{
  gint total_size;
  gchar *evt_str;
  gint str_len;
  const GstStructure *e_structure = gst_event_get_structure (event);

  evt_str = gst_structure_to_string (e_structure);
  str_len = strlen (evt_str)+1;
  total_size = str_len + 16;

  if (total_size > size) {
    goto finalize;
  }

  gst_debugger_protocol_utils_serialize_integer64 (event->type, buffer, 4);
  gst_debugger_protocol_utils_serialize_uinteger64(event->timestamp, buffer+4, 8);
  gst_debugger_protocol_utils_serialize_uinteger64(event->seqnum, buffer+12, 4);
  memcpy (buffer + 16, evt_str, str_len);

finalize:
  g_free (evt_str);
  return total_size;
}

gint gst_message_serialize(GstMessage * message, gchar * buffer, gint size)
{
  gint total_size;
  gchar *msg_str;
  gint msg_str_len;
  const GstStructure *m_structure = gst_message_get_structure (message);
  gchar *src_name = GST_MESSAGE_SRC_NAME (message);
  gint src_name_len = strlen (src_name) + 1;

  msg_str = gst_structure_to_string (m_structure);
  msg_str_len = strlen (msg_str)+1;
  total_size = msg_str_len + 16 + src_name_len;

  if (total_size > size) {
    goto finalize;
  }

  gst_debugger_protocol_utils_serialize_integer64 (message->type, buffer, 4);
  gst_debugger_protocol_utils_serialize_uinteger64(message->timestamp, buffer+4, 8);
  gst_debugger_protocol_utils_serialize_uinteger64(message->seqnum, buffer+12, 4);

  memcpy (buffer + 16, msg_str, msg_str_len);
  memcpy (buffer + 16 + msg_str_len, src_name, src_name_len);

finalize:
  g_free (msg_str);
  return total_size;
}

gint gst_buffer_serialize(GstBuffer * gstbuffer, gchar * buffer, gint size)
{
  gint buff_size = gst_buffer_get_size(gstbuffer);
  gint total_size = 44 + buff_size;

  if (total_size > size) {
    goto finalize;
  }

  gst_debugger_protocol_utils_serialize_uinteger64 (GST_BUFFER_PTS (gstbuffer), buffer, 8);
  gst_debugger_protocol_utils_serialize_uinteger64 (GST_BUFFER_DTS (gstbuffer), buffer+8, 8);
  gst_debugger_protocol_utils_serialize_uinteger64 (GST_BUFFER_DURATION (gstbuffer), buffer+16, 8);
  gst_debugger_protocol_utils_serialize_uinteger64 (GST_BUFFER_OFFSET (gstbuffer), buffer+24, 8);
  gst_debugger_protocol_utils_serialize_uinteger64 (GST_BUFFER_OFFSET_END (gstbuffer), buffer+32, 8);
  gst_debugger_protocol_utils_serialize_uinteger64 (GST_BUFFER_FLAGS (gstbuffer), buffer+40, 4);

  gst_buffer_extract(gstbuffer, 0, buffer + 44, buff_size);

finalize:
  return total_size;
}

gchar * g_value_serialize (GValue * value, GType * type, InternalGType * internal_type)
{
  GValue tmp = G_VALUE_INIT;

  if (G_TYPE_IS_FUNDAMENTAL (value->g_type)) {
    *type = value->g_type;
    *internal_type = INTERNAL_GTYPE_FUNDAMENTAL;
    return gst_value_serialize (value);
  } else if (G_TYPE_IS_ENUM(value->g_type)) {
    g_value_init(&tmp, G_TYPE_INT);
    *type = G_TYPE_INT;
    g_value_set_int(&tmp, g_value_get_enum(value));
    *internal_type = INTERNAL_GTYPE_ENUM;
  } else if (value->g_type == GST_TYPE_CAPS) {
    g_value_init(&tmp, G_TYPE_STRING);
    *type = G_TYPE_STRING;
    g_value_set_string (&tmp, gst_caps_to_string (gst_value_get_caps (value)));
    *internal_type = INTERNAL_GTYPE_CAPS;
  } else if (value->g_type == GST_TYPE_OBJECT) {
    g_value_init(&tmp, G_TYPE_STRING);
    gchar buffer[128];
    snprintf (buffer, 128, "(GstObject:name) %s", GST_OBJECT_NAME (g_value_get_object (value)));
    *type = G_TYPE_STRING;
    g_value_set_string (&tmp, g_strdup (buffer));
    *internal_type = INTERNAL_GTYPE_GST_OBJECT;
  }

  return gst_value_serialize (&tmp);
}
