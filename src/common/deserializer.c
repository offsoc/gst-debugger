/*
 * deserializer.c
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "deserializer.h"
#include "protocol_utils.h"

#include "gst-utils.h"

#include <string.h>
#include <stdlib.h>

GstQuery* gst_query_deserialize (const gchar * buffer, gint size)
{
  GstQueryType type;
  GstStructure *structure;

  type = (GstQueryType)gst_debugger_protocol_utils_deserialize_integer64 (buffer, 4);

  structure = gst_structure_from_string (buffer+4, NULL);

  return gst_query_new_custom (type, structure);
}

GstEvent* gst_event_deserialize (const gchar * buffer, gint size)
{
  GstEventType type;
  guint64 timestamp;
  guint32 seqnum;
  GstEvent *event;
  GstStructure *e_structure;

  type = (GstEventType)gst_debugger_protocol_utils_deserialize_integer64 (buffer, 4);
  timestamp = gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+4, 8);
  seqnum = gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+12, 4);
  e_structure = gst_structure_from_string (buffer+16, NULL);
  event = gst_event_new_custom (type, e_structure);
  gst_event_set_seqnum (event, seqnum);
  event->timestamp = timestamp;

  return event;
}

GstMessage* gst_message_deserialize (const gchar * buffer, gint size)
{
  GstMessageType type;
  guint64 timestamp;
  guint32 seqnum;
  GstMessage *message;
  GstStructure *m_structure;
  gint pos;

  type = (GstMessageType)gst_debugger_protocol_utils_deserialize_integer64 (buffer, 4);
  timestamp = gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+4, 8);
  seqnum = gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+12, 4);
  m_structure = gst_structure_from_string (buffer+16, NULL);
  pos = strlen (buffer+16) + 16 + 1;
  gst_structure_set (m_structure, "message source", G_TYPE_STRING, buffer + pos, NULL);
  message = gst_message_new_custom (type, NULL, m_structure);
  gst_message_set_seqnum (message, seqnum);
  message->timestamp = timestamp;

  return message;
}

GstBuffer* gst_buffer_deserialize (const gchar * buffer, gint size)
{
  GstBuffer *gstbuffer;
  gchar *data;

  data = (gchar*) malloc (size);
  memcpy (data, buffer+44, size-44);

  gstbuffer = gst_buffer_new_wrapped (data, size - 44);

  GST_BUFFER_PTS (gstbuffer) =
    gst_debugger_protocol_utils_deserialize_uinteger64 (buffer, 8);
  GST_BUFFER_DTS (gstbuffer) =
    gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+8, 8);
  GST_BUFFER_DURATION (gstbuffer) =
    gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+16, 8);
  GST_BUFFER_OFFSET (gstbuffer) =
    gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+24, 8);
  GST_BUFFER_OFFSET_END (gstbuffer) =
    gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+32, 8);
  GST_BUFFER_FLAGS (gstbuffer) =
    gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+40, 4);

  return gstbuffer;
}

void g_value_deserialize (GValue * value, GType type, InternalGType internal_type, const gchar * data)
{
  switch (internal_type) {
  case INTERNAL_GTYPE_ENUM:
  case INTERNAL_GTYPE_FLAGS:
  {
    GValue tmp = G_VALUE_INIT;
    g_value_init (&tmp, type);
    gst_value_deserialize (&tmp, data);
    if (internal_type == INTERNAL_GTYPE_ENUM) {
      g_value_init(value, gst_utils_get_virtual_enum_type ());
      g_value_set_enum (value, g_value_get_int (&tmp));
    } else {
      g_value_init(value, gst_utils_get_virtual_flags_type ());
      g_value_set_flags (value, g_value_get_int (&tmp));
    }
    g_value_unset (&tmp);
    break;
  }
  case INTERNAL_GTYPE_GST_OBJECT: // and with pointers
  case INTERNAL_GTYPE_FUNDAMENTAL:
  case INTERNAL_GTYPE_UNKNOWN:
    g_value_init (value, type);
    gst_value_deserialize (value, data);
    break;
  case INTERNAL_GTYPE_CAPS:
  {
    GstCaps *caps;
    g_value_init (value, G_TYPE_STRING);
    gst_value_deserialize (value, data);
    caps = gst_caps_from_string (g_value_get_string (value));
    g_value_unset (value);
    g_value_init (value, GST_TYPE_CAPS);
    gst_value_set_caps (value, caps);
    break;
  }
  }
}
