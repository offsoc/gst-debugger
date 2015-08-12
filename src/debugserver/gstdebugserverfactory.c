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

#include "gstdebugserverfactory.h"

#include "protocol/gstdebugger.pb-c.h"

#include <gst/gst.h>

gint gst_debugserver_factory_prepare_buffer (const gchar * factory_name, gchar * buffer, gint max_size)
{
  GstElementFactory *factory = gst_element_factory_find (factory_name);
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  FactoryInfo f_info = FACTORY_INFO__INIT;
  TopologyTemplate **templates = NULL;
  gint i = 0, size;
  GList *tpls = NULL;
  GstStaticPadTemplate *static_template = NULL;

  if (factory == NULL) {
    // todo
  }

  info.info_type = GSTREAMER_INFO__INFO_TYPE__FACTORY;
  f_info.name = g_strdup (factory_name);
  f_info.n_templates = gst_element_factory_get_num_pad_templates (factory);

  if (f_info.n_templates != 0) {
    templates = g_malloc (f_info.n_templates * sizeof (TopologyTemplate*));
    tpls = (GList*)gst_element_factory_get_static_pad_templates (factory);

    while (tpls) {
      static_template = (GstStaticPadTemplate*) tpls->data;
      tpls = g_list_next (tpls);
      templates[i] = g_malloc (sizeof (TopologyTemplate));
      topology_template__init (templates[i]);
      templates[i]->caps = g_strdup (static_template->static_caps.string);
      templates[i]->direction = static_template->direction;
      templates[i]->presence = static_template->presence;
      templates[i]->name_template = g_strdup (static_template->name_template);
      i++;
    }
  }

  f_info.templates = templates;

  // todo
  f_info.n_interfaces = 0;
  f_info.interfaces = NULL;
  f_info.n_protocols = 0;
  f_info.protocols = NULL;

  info.factory_info = &f_info;

  size = gstreamer_info__get_packed_size (&info);

  if (max_size < size) {
    goto finalize;
  }

  gstreamer_info__pack (&info, (guint8*)buffer);

finalize:
  for (i = 0; i < (gint) f_info.n_templates; i++) {
    g_free (templates[i]);
  }
  g_free (templates);

  return size;
}


