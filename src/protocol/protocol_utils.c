#include "protocol_utils.h"

gboolean
gst_debugger_protocol_utils_read_requested_size (GInputStream * istream, gint requested_size, guint8 * buffer)
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
  guint8 buffer[4];
  gint size = 0;
  gint i;

  if (gst_debugger_protocol_utils_read_requested_size(istream, 4, buffer) == FALSE) {
    return -1;
  }

  for (i = 0; i < 4; i++)
    size |= buffer[i] << i;

  return size;
}
