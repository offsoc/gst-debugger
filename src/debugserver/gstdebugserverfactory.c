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

#include "common/gstdebugger.pb-c.h"

gint gst_debugserver_factory_prepare_buffer (GstElementFactory *factory, gchar * buffer, gint max_size)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  FactoryInfo f_info = FACTORY_INFO__INIT;
  TopologyTemplate **templates = NULL;
  gint i = 0, size;
  GList *tpls = NULL;
  GstStaticPadTemplate *static_template = NULL;

  info.info_type = GSTREAMER_INFO__INFO_TYPE__FACTORY;
  f_info.name = gst_plugin_feature_get_name (factory);
  f_info.n_templates = gst_element_factory_get_num_pad_templates (factory);

  if (f_info.n_templates != 0) {
    templates = g_malloc (f_info.n_templates * sizeof (TopologyTemplate*));
    tpls = (GList*)gst_element_factory_get_static_pad_templates (factory);

    while (tpls) {
      static_template = (GstStaticPadTemplate*) tpls->data;
      tpls = g_list_next (tpls);
      templates[i] = g_malloc (sizeof (TopologyTemplate));
      topology_template__init (templates[i]);
      templates[i]->caps = (gchar*) static_template->static_caps.string;
      templates[i]->direction = static_template->direction;
      templates[i]->presence = static_template->presence;
      templates[i]->name_template = (gchar*) static_template->name_template;
      i++;
    }
  }

  f_info.templates = templates;

  gchar **keys, **k;
  int meta_cnt = 0;
  i = 0;
  FactoryMetaEntry **entries = NULL;
  keys = gst_element_factory_get_metadata_keys (factory);
  if (keys != NULL) {
    for (k = keys; *k != NULL; ++k) { meta_cnt++; }
    entries = g_malloc (sizeof (FactoryMetaEntry*) * meta_cnt);
    for (k = keys; *k != NULL; ++k) {
      entries[i] = g_malloc (sizeof (FactoryMetaEntry));
      factory_meta_entry__init (entries[i]);
      entries[i]->key = *k;
      entries[i]->value = (gchar*) gst_element_factory_get_metadata (factory, *k);
      i++;
    }
  }

  f_info.meta_entries = entries;
  f_info.n_meta_entries = meta_cnt;

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

  for (i = 0; i < (gint) f_info.n_meta_entries; i++) {
    g_free (entries[i]);
  }
  g_free (entries);
  g_strfreev (keys);

  return size;
}


