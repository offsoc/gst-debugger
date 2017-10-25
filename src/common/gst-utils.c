#include "gst-utils.h"

#include <assert.h>
#include <string.h>

static gpointer
gst_unknown_type_copy (gpointer * object)
{
  return gst_unknown_type_new (((GstUnknownType *) object)->type_name);
}

static void
gst_unknown_type_free (gpointer * object)
{
  g_free (((GstUnknownType *) object)->type_name);
  g_slice_free1 (sizeof (GstUnknownType), object);
}

G_DEFINE_BOXED_TYPE (GstUnknownType, gst_unknown_type,
    (GBoxedCopyFunc) gst_unknown_type_copy,
    (GBoxedFreeFunc) gst_unknown_type_free);

GstUnknownType *
gst_unknown_type_new (const gchar * type_name)
{
  GstUnknownType *self = g_slice_new (GstUnknownType);
  self->type_name = g_strdup (type_name);
  return self;
}

GstElement *
gst_utils_get_element_from_path (GstElement * root, const gchar * path)
{
  if (root == NULL || path == NULL) {
    return NULL;
  }

  assert (path[0] == '/');

  if (strlen (path) == 1) {
    return root;
  }

  gchar **element_names = g_strsplit (path, "/", -1);

  gint i, size;
  GstElement *sp = root;

  for (size = 0; element_names && element_names[size]; size++);

  for (i = 0; i < size - 1; i++) {
    if (strlen (element_names[i]) == 0) {
      continue;
    }
    if (GST_IS_BIN (sp)) {
      sp = gst_bin_get_by_name (GST_BIN (sp), element_names[i]);
    } else {
      g_strfreev (element_names);
      return NULL;
    }
  }

  sp = gst_bin_get_by_name (GST_BIN (sp), element_names[size - 1]);
  g_strfreev (element_names);

  return sp;
}

GstPad *
gst_utils_get_pad_from_path (GstElement * root, const gchar * pad_path)
{
  if (pad_path == NULL || strlen (pad_path) == 0) {
    return NULL;
  }

  GstPad *pad = NULL;
  gchar *tmp_path = g_strdup (pad_path);
  gchar *pad_name = strrchr (tmp_path, ':');
  GstElement *parent_element = NULL;

  if (pad_name == NULL) {
    goto finalize;
  }

  *pad_name = 0;
  pad_name++;

  parent_element = gst_utils_get_element_from_path (root, tmp_path);

  if (parent_element == NULL) {
    goto finalize;
  }

  pad = gst_element_get_static_pad (parent_element, pad_name);

finalize:
  g_free (tmp_path);
  return pad;
}

gchar *
gst_utils_get_object_path (GstObject * obj)
{
  GString *path;

  assert (obj != NULL);

  const gchar *init =
      gst_object_get_parent (obj) == NULL ? NULL : GST_OBJECT_NAME (obj);

  path = g_string_new (init);

  if (GST_IS_PAD (obj)) {
    g_string_prepend_c (path, ':');
  } else if (GST_IS_ELEMENT (obj)) {
    g_string_prepend_c (path, '/');
  } else {
    assert (FALSE);             // only GstElement and GstPad allowed
  }

  obj = gst_object_get_parent (obj);

  while (obj != NULL && gst_object_get_parent (obj) != NULL) {
    g_string_prepend (path, GST_OBJECT_NAME (obj));
    g_string_prepend_c (path, '/');
    obj = gst_object_get_parent (obj);
  }

  return g_string_free (path, FALSE);
}

gboolean
gst_utils_check_pad_has_element_parent (GstPad * pad)
{
  GstObject *obj = GST_OBJECT_PARENT (pad);
  if (obj == NULL) {
    return FALSE;
  } else if (GST_IS_ELEMENT (obj)) {
    return TRUE;
  } else if (GST_IS_PAD (obj)) {        // internal pad
    obj = GST_OBJECT_PARENT (obj);
    return obj != NULL && GST_IS_ELEMENT (obj);
  }
  return FALSE;
}

GType
gst_utils_get_virtual_enum_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    {0, "DUMMY_VALUE_NAME_BEGIN", "dummy nick name begin"},
    {INT_MAX, "DUMMY_VALUE_NAME_END", "dummy nick name end"},
    {0, NULL, NULL}
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("GstDebuggerVirtualEnumType", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
gst_utils_get_virtual_flags_type (void)
{
  static gsize id = 0;
  static const GFlagsValue values[] = {
    {0, "DUMMY_VALUE_NAME_BEGIN", "dummy nick name begin"},
    {INT_MAX, "DUMMY_VALUE_NAME_END", "dummy nick name end"},
    {0, NULL, NULL}
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_flags_register_static ("GstDebuggerVirtualFlagsType", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}
