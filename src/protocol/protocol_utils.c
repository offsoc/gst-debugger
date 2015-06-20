#include "protocol_utils.h"

void
gst_debugger_protocol_utils_serialize_integer (gint value, gchar * buffer, gint size)
{
  gint i;
  for (i = 0; i < size; i++) {
    buffer[i] = value % 256;
    value /= 256;
  }
}

gint
gst_debugger_protocol_utils_deserialize_integer (gchar * buffer, gint size)
{
  gint value = 0, i;

  for (i = size - 1; i >= 0; i--) {
    value *= 256;
    value += buffer[i];
  }

  return value;
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

  return gst_debugger_protocol_utils_deserialize_integer (buffer, 4);
}
