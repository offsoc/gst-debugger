#include "gst-utils.h"

#include <glib.h>

#include <string.h>

GstPad* gst_utils_find_pad (GstPipeline * start, gchar * pad_path)
{
  if (pad_path == NULL || strlen (pad_path) == 0) {
    return NULL;
  }

  gchar ** tokens = g_strsplit (pad_path, ":", -1);
  gint i, size;
  GstElement * sp = GST_ELEMENT (start);
  GstPad * pad;

  for (size = 0; tokens && tokens[size]; size++);

  for (i = 0; i < size-1; i++) {
    if (GST_IS_BIN (sp)) {
      sp = gst_bin_get_by_name (GST_BIN (sp), tokens[i]);
    } else if (i != size-2) {
      g_strfreev (tokens);
      return NULL;
    }
  }

  pad = gst_element_get_static_pad (sp, tokens[i]);
  g_strfreev (tokens);

  return pad;
}
