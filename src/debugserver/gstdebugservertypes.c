/* GStreamer
 * Copyright (C) 2015 Marcin Kolny <marcin.kolny@gmail.com>
 *
 * gstdebugserver.c: tracing module that sends serialized data to
 * an user.
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

#include "gstdebugservertypes.h"

#include "common/serializer.h"
#include "common/common.h"

#include <gst/gst.h>

#include <string.h>

#define SERIALIZE_ENUM_FLAGS \
  do { \
    n_values = klass->n_values; \
    values = g_malloc (sizeof (GstDebugger__EnumFlagsValue*) * n_values); \
    \
    for (i = 0; i < klass->n_values; i++) { \
      values[i] = (GstDebugger__EnumFlagsValue*) g_malloc (sizeof (GstDebugger__EnumFlagsValue)); \
      gst_debugger__enum_flags_value__init (values[i]); \
      values[i]->name = (gchar*) klass->values[i].value_name; \
      values[i]->nick = (gchar*) klass->values[i].value_nick; \
      values[i]->value = klass->values[i].value; \
    } \
    \
    data_type.values = values; \
    data_type.n_values = klass->n_values; \
    data_type.type_name = (gchar*) name; \
    gst_data.enum_flags_type = &data_type; \
  } while (FALSE)

static void gst_debugserver_types_send_enum_flags (GstDebugserverTcp *tcp_server, TcpClient *client, const gchar * name)
{
  GType type = g_type_from_name (name);
  GstDebugger__GStreamerData gst_data = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__EnumFlagsType data_type = GST_DEBUGGER__ENUM_FLAGS_TYPE__INIT;
  GstDebugger__EnumFlagsValue **values = NULL;
  gint i = 0, n_values = 0;

  if (G_TYPE_IS_ENUM (type)) {
    GEnumClass * klass = g_type_class_peek (type);
    SERIALIZE_ENUM_FLAGS;
    data_type.kind = GST_DEBUGGER__ENUM_FLAGS_TYPE__ENUM_FLAGS_KIND__ENUM;
  } else if (G_TYPE_IS_FLAGS (type)) {
    GFlagsClass * klass = g_type_class_peek (type);
    SERIALIZE_ENUM_FLAGS;
    data_type.kind = GST_DEBUGGER__ENUM_FLAGS_TYPE__ENUM_FLAGS_KIND__FLAGS;
  } else {
    // todo
  }

  gst_data.enum_flags_type = &data_type;
  gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_ENUM_FLAGS_TYPE;

  gst_debugserver_tcp_send_packet (tcp_server, client, &gst_data);

  for (i = 0; i < n_values; i++) {
    g_free (values[i]);
  }
  g_free (values);
}

static void gst_debugserver_types_send_factory (GstDebugserverTcp *tcp_server, TcpClient *client, const gchar * name)
{
  GstDebugger__GStreamerData gst_data = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__FactoryType factory_type = GST_DEBUGGER__FACTORY_TYPE__INIT;
  GstDebugger__PadTemplate **templates = NULL;
  GstElementFactory *factory = gst_element_factory_find (name);
  GstStaticPadTemplate *static_template = NULL;
  GList *tpls = NULL;
  gint i = 0;

  if (factory == NULL) {
    // todo
    return;
  }

  factory_type.name = gst_plugin_feature_get_name (factory);
  factory_type.n_templates = gst_element_factory_get_num_pad_templates (factory);
  if (factory_type.n_templates != 0) {
    templates = g_malloc (factory_type.n_templates * sizeof (GstDebugger__PadTemplate*));
    tpls = (GList*)gst_element_factory_get_static_pad_templates (factory);

    while (tpls) {
      static_template = (GstStaticPadTemplate*) tpls->data;
      tpls = g_list_next (tpls);
      templates[i] = g_malloc (sizeof (GstDebugger__PadTemplate));
      gst_debugger__pad_template__init (templates[i]);
      templates[i]->caps = (gchar*) static_template->static_caps.string;
      templates[i]->direction = static_template->direction;
      templates[i]->presence = static_template->presence;
      templates[i]->name_template = (gchar*) static_template->name_template;
      i++;
    }
  }
  factory_type.templates = templates;

  gchar **keys, **k;
  gint meta_cnt = 0;
  i = 0;
  GstDebugger__FactoryMeta **entries = NULL;
  keys = gst_element_factory_get_metadata_keys (factory);
  if (keys != NULL) {
    for (k = keys; *k != NULL; ++k) { meta_cnt++; }
    entries = g_malloc (sizeof (GstDebugger__FactoryMeta*) * meta_cnt);
    for (k = keys; *k != NULL; ++k) {
      entries[i] = g_malloc (sizeof (GstDebugger__FactoryMeta));
      gst_debugger__factory_meta__init (entries[i]);
      entries[i]->key = *k;
      entries[i]->value = (gchar*) gst_element_factory_get_metadata (factory, *k);
      i++;
    }
  }
  factory_type.metadata = entries;
  factory_type.n_metadata = meta_cnt;

  gst_data.factory = &factory_type;
  gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_FACTORY;

  gst_debugserver_tcp_send_packet (tcp_server, client, &gst_data);

  for (i = 0; i < (gint) factory_type.n_templates; i++) {
    g_free (templates[i]);
  }
  g_free (templates);

  for (i = 0; i < (gint) factory_type.n_metadata; i++) {
    g_free (entries[i]);
  }
  g_free (entries);
  g_strfreev (keys);
}

static void gst_debugserver_types_send_klass (GstDebugserverTcp *tcp_server, TcpClient *client, const gchar * name)
{
  GType type = g_type_from_name (name);
  GObjectClass *obj_klass = G_OBJECT_CLASS (g_type_class_peek (type));
  GstElementClass *element_klass = GST_ELEMENT_CLASS (obj_klass);
  GstDebugger__GStreamerData gst_data = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__ElementKlass klass = GST_DEBUGGER__ELEMENT_KLASS__INIT;
  GstDebugger__PropertyInfo **properties_info = NULL;
  GParamSpec **specs;
  GValue gvalue = G_VALUE_INIT;
  gint n_specs, i;
  GstElement *element = NULL;
  GType out_gtype;
  InternalGType out_internal_type;
  GstDebugger__Value *value = NULL;

  if (element_klass == NULL) {
    // todo
    return;
  }

  klass.name = (gchar*) name;
  specs = g_object_class_list_properties (obj_klass, &n_specs);
  klass.n_property_info = n_specs;

  properties_info = g_malloc (sizeof (GstDebugger__PropertyInfo*) * n_specs);

  for (i = 0; i < n_specs; i++) {
    properties_info[i] = g_malloc (sizeof (GstDebugger__PropertyInfo));
    gst_debugger__property_info__init (properties_info[i]);
    properties_info[i]->blurb = (gchar*) g_param_spec_get_blurb (specs[i]);
    properties_info[i]->flags = specs[i]->flags;
    properties_info[i]->name = (gchar*) g_param_spec_get_name (specs[i]);
    properties_info[i]->nick = (gchar*) g_param_spec_get_nick (specs[i]);

    g_value_init (&gvalue, specs[i]->value_type);
    g_param_value_set_default (specs[i], &gvalue);
    value = (GstDebugger__Value*) g_malloc (sizeof (GstDebugger__Value));
    gst_debugger__value__init (value);
    value->type_name = (gchar*) g_type_name (specs[i]->value_type);
    value->data.data = (uint8_t*) g_value_serialize (&gvalue, &out_gtype, &out_internal_type);
    value->data.len = value->data.data == NULL ? 0 : strlen (value->data.data);
    value->gtype = out_gtype;

    if (out_gtype == specs[i]->value_type) {
      value->internal_type = out_internal_type;
      value->has_internal_type = TRUE;
    } else {
      value->has_internal_type = FALSE;
    }
    properties_info[i]->default_value = value;

    g_value_unset (&gvalue);
  }

  klass.property_info = properties_info;
  gst_data.element_klass = &klass;
  gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_ELEMENT_KLASS;

  gst_debugserver_tcp_send_packet (tcp_server, client, &gst_data);

  for (i = 0; i < (gint) n_specs; i++) {
    g_free (properties_info[i]->default_value);
    g_free (properties_info[i]);
  }

  g_free (properties_info);
}

void gst_debugserver_types_send_type (GstDebugserverTcp *tcp_server, TcpClient *client, const GstDebugger__TypeDescriptionRequest *request)
{
  switch (request->type) {
  case GST_DEBUGGER__TYPE_DESCRIPTION_REQUEST__TYPE__FACTORY:
    gst_debugserver_types_send_factory (tcp_server, client, request->name);
    break;
  case GST_DEBUGGER__TYPE_DESCRIPTION_REQUEST__TYPE__ENUM_FLAGS:
    gst_debugserver_types_send_enum_flags (tcp_server, client, request->name);
    break;
  case GST_DEBUGGER__TYPE_DESCRIPTION_REQUEST__TYPE__KLASS:
    gst_debugserver_types_send_klass (tcp_server, client, request->name);
    break;
  }
}
