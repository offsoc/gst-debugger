#include "protocol_utils.h"

#define SESRIALIZER \
  gint i; \
  for (i = 0; i < size; i++) { \
    buffer[i] = value % 256; \
    value /= 256; \
  }

#define DESERIALIZER \
  gint value = 0, i; \
  for (i = size - 1; i >= 0; i--) { \
    value *= 256; \
    value += (unsigned char)buffer[i]; \
  } \
  return value;

void
gst_debugger_protocol_utils_serialize_integer64 (gint64 value, gchar * buffer, gint size)
{
  SESRIALIZER
}

void
gst_debugger_protocol_utils_serialize_uinteger64 (guint64 value, gchar * buffer, gint size)
{
  SESRIALIZER
}

gint64
gst_debugger_protocol_utils_deserialize_integer64 (const gchar * buffer, gint size)
{
  DESERIALIZER
}

guint64
gst_debugger_protocol_utils_deserialize_uinteger64 (const gchar * buffer, gint size)
{
  DESERIALIZER
}

gboolean
gst_debugger_protocol_utils_read_requested_size (GInputStream * istream, gint requested_size, gchar * buffer)
{
  gint size = 0;
  gint cnt = 0;

  while (size < requested_size) {
    if ((cnt = g_input_stream_read (istream, buffer + size, requested_size - size, NULL, NULL)) < 1) {
      return FALSE;
    }
    size += cnt;
  }

  return TRUE;
}

gint
gst_debugger_protocol_utils_read_header (GInputStream * istream)
{
  gchar buffer[4];

  if (gst_debugger_protocol_utils_read_requested_size(istream, 4, buffer) == FALSE) {
    return -1;
  }

  return gst_debugger_protocol_utils_deserialize_integer64 (buffer, 4);
}
