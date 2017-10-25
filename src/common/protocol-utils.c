#include "protocol-utils.h"

void
gst_debugger_protocol_utils_serialize_integer64 (gint64 value, gchar * buffer,
    gint size)
{
  gint i;
  for (i = 0; i < size; i++) {
    buffer[i] = value % 256;
    value /= 256;
  }
}

static gint64
gst_debugger_protocol_utils_deserialize_integer64 (const gchar * buffer,
    gint size)
{
  gint value = 0, i;
  for (i = size - 1; i >= 0; i--) {
    value *= 256;
    value += (unsigned char) buffer[i];
  }
  return value;
}

gboolean
gst_debugger_protocol_utils_read_requested_size (GInputStream * istream,
    gint requested_size, gchar * buffer, GCancellable * cancel)
{
  gint size = 0;
  gint cnt = 0;

  while (size < requested_size) {
    if ((cnt =
            g_input_stream_read (istream, buffer + size, requested_size - size,
                cancel, NULL)) < 1) {
      return FALSE;
    }
    size += cnt;
  }

  return TRUE;
}

gint
gst_debugger_protocol_utils_read_header (GInputStream * istream,
    GCancellable * cancel)
{
  gchar buffer[4];

  if (gst_debugger_protocol_utils_read_requested_size (istream, 4, buffer,
          cancel) == FALSE) {
    return -1;
  }

  return gst_debugger_protocol_utils_deserialize_integer64 (buffer, 4);
}

GError *
gst_debugger_protocol_write_header (GOutputStream * ostream, gint64 value)
{
  gchar size_buffer[4];
  GError *err = NULL;

  gst_debugger_protocol_utils_serialize_integer64 (value, size_buffer, 4);
  g_output_stream_write (ostream, size_buffer, 4, NULL, &err);

  return err;
}
