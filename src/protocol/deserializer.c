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

  type = (GstQueryType)gst_debugger_protocol_utils_deserialize_integer (buffer, 4);

  structure = gst_structure_from_string (buffer+4, NULL);

  return gst_query_new_custom (type, structure);
}
