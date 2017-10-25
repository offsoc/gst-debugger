/*
 * serializer.c
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "serializer.h"
#include <stdio.h>
#include "gst-utils.h"

gchar *
g_value_serialize (GValue * value, GType * type, InternalGType * internal_type)
{
  GValue tmp = G_VALUE_INIT;
  gchar *serialized_value = NULL;

  if (G_TYPE_IS_FUNDAMENTAL (value->g_type)
      && value->g_type < G_TYPE_RESERVED_USER_FIRST) {
    *type = value->g_type;
    *internal_type = INTERNAL_GTYPE_FUNDAMENTAL;
    return gst_value_serialize (value);
  } else if (G_TYPE_IS_ENUM (value->g_type) || G_TYPE_IS_FLAGS (value->g_type)) {
    gint val;
    g_value_init (&tmp, G_TYPE_INT);
    *type = G_TYPE_INT;
    if (G_TYPE_IS_FLAGS (value->g_type)) {
      val = g_value_get_flags (value);
      *internal_type = INTERNAL_GTYPE_FLAGS;
    } else {
      val = g_value_get_enum (value);
      *internal_type = INTERNAL_GTYPE_ENUM;
    }
    g_value_set_int (&tmp, val);
  } else if (value->g_type == GST_TYPE_CAPS) {
    g_value_init (&tmp, G_TYPE_STRING);
    *type = G_TYPE_STRING;
    g_value_take_string (&tmp, gst_caps_to_string (gst_value_get_caps (value)));
    *internal_type = INTERNAL_GTYPE_CAPS;
  } else if (value->g_type == GST_TYPE_OBJECT) {
    g_value_init (&tmp, G_TYPE_STRING);
    gchar buffer[128];
    GstObject *obj = g_value_get_object (value);
    gchar *name;
    if (obj == NULL || GST_OBJECT_NAME (obj) == NULL) {
      name = "(null)";
    } else {
      name = GST_OBJECT_NAME (obj);
    }
    snprintf (buffer, 128, "(GstObject:name) %s", name);
    *type = G_TYPE_STRING;
    g_value_set_string (&tmp, buffer);
    *internal_type = INTERNAL_GTYPE_GST_OBJECT;
  } else {
    g_value_init (&tmp, G_TYPE_STRING);
    gchar buffer[128];
    snprintf (buffer, 128, "%s", g_type_name (value->g_type));
    *type = G_TYPE_STRING;
    *internal_type = INTERNAL_GTYPE_UNKNOWN;
    g_value_set_string (&tmp, buffer);
  }

  serialized_value = gst_value_serialize (&tmp);
  g_value_unset (&tmp);

  return serialized_value;
}


void
g_value_deserialize (GValue * value, GType type, InternalGType internal_type,
    const gchar * data, gint len)
{
  // I don't use string for Value::data field, because serializer implementation
  // might change in the future (I don't break the API then).
  gchar *t_data = g_strndup (data, len);
  switch (internal_type) {
    case INTERNAL_GTYPE_ENUM:
    case INTERNAL_GTYPE_FLAGS:
    {
      GValue tmp = G_VALUE_INIT;
      g_value_init (&tmp, type);
      gst_value_deserialize (&tmp, t_data);
      if (internal_type == INTERNAL_GTYPE_ENUM) {
        g_value_init (value, gst_utils_get_virtual_enum_type ());
        g_value_set_enum (value, g_value_get_int (&tmp));
      } else {
        g_value_init (value, gst_utils_get_virtual_flags_type ());
        g_value_set_flags (value, g_value_get_int (&tmp));
      }
      g_value_unset (&tmp);
      break;
    }
    case INTERNAL_GTYPE_GST_OBJECT:    // and with pointers
    case INTERNAL_GTYPE_FUNDAMENTAL:
      g_value_init (value, type);
      gst_value_deserialize (value, t_data);
      break;
    case INTERNAL_GTYPE_UNKNOWN:
    {
      GValue tmp = G_VALUE_INIT;
      g_value_init (&tmp, G_TYPE_STRING);
      gst_value_deserialize (&tmp, t_data);
      g_value_init (value, gst_unknown_type_get_type ());
      g_value_take_boxed (value,
          gst_unknown_type_new (g_value_get_string (&tmp)));
      g_value_unset (&tmp);

      break;
    }
    case INTERNAL_GTYPE_CAPS:
    {
      GstCaps *caps;
      g_value_init (value, G_TYPE_STRING);
      gst_value_deserialize (value, t_data);
      caps = gst_caps_from_string (g_value_get_string (value));
      g_value_unset (value);
      g_value_init (value, GST_TYPE_CAPS);
      gst_value_set_caps (value, caps);
      gst_caps_unref (caps);
      break;
    }
  }

  g_free (t_data);
}
