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

gint gst_query_serialize (GstQuery * query, guint8 * buffer, gint size)
{
  const GstStructure * q_structure;
  gchar * str;
  gint str_size;
  gint type_size;
  gint total_size;
  q_structure = gst_query_get_structure (query);

  str = gst_structure_to_string (q_structure);
  str_size = strlen (str);
  type_size = 4;
  total_size = type_size + str_size + 4;
  if (total_size > size) {
    goto finalize;
  }
  gst_debugger_protocol_utils_serialize_integer (total_size, buffer, 4);

  gst_debugger_protocol_utils_serialize_integer (query->type, buffer + 4, type_size);
  memcpy (buffer + type_size + 4, str, str_size);

finalize:
  g_free (str);
  return total_size;

}
