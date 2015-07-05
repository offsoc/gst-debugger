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
  gint str_len;
  const GstStructure *m_structure = gst_message_get_structure (message);

  msg_str = gst_structure_to_string (m_structure);
  str_len = strlen (msg_str)+1;
  total_size = str_len + 16;

  if (total_size > size) {
    goto finalize;
  }

  gst_debugger_protocol_utils_serialize_integer64 (message->type, buffer, 4);
  gst_debugger_protocol_utils_serialize_uinteger64(message->timestamp, buffer+4, 8);
  // todo object path (GstMessage::src)
  gst_debugger_protocol_utils_serialize_uinteger64(message->seqnum, buffer+12, 4);

  memcpy (buffer + 16, msg_str, str_len);

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
