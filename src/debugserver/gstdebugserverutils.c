/* GStreamer
 * Copyright (C) 2015 Marcin Kolny <marcin.kolny@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "gstdebugserverutils.h"
#include <glib.h>

GstPad* gst_debugserver_utils_find_pad (GstPipeline * start, gchar * pad_path)
{
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
