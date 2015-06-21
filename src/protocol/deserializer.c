/*
 * deserializer.c
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "deserializer.h"
#include "protocol_utils.h"

#include <string.h>

GstQuery* gst_query_deserialize (gchar * buffer, gint size)
{
  GstQueryType type;
  GstStructure *structure;

  if (strlen (buffer+4) + 5 != (guint)size) {
    return NULL;
  }

  type = (GstQueryType)gst_debugger_protocol_utils_deserialize_integer64 (buffer, 4);

  structure = gst_structure_from_string (buffer+4, NULL);

  return gst_query_new_custom (type, structure);
}

GstEvent* gst_event_deserialize (gchar * buffer, gint size)
{
  GstEventType type;
  guint64 timestamp;
  guint32 seqnum;
  GstEvent *event;
  GstStructure *e_structure;

  if (strlen (buffer+16) + 17 != (guint)size) {
    return NULL;
  }

  type = (GstEventType)gst_debugger_protocol_utils_deserialize_integer64 (buffer, 4);
  timestamp = gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+4, 8);
  seqnum = gst_debugger_protocol_utils_deserialize_uinteger64 (buffer+12, 4);
  e_structure = gst_structure_from_string (buffer+16, NULL);
  event = gst_event_new_custom (type, e_structure);
  gst_event_set_seqnum (event, seqnum);
  event->timestamp = timestamp;

  return event;
}

