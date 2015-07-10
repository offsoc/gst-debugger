#include "gst-utils.h"

#include <glib.h>

#include <assert.h>
#include <string.h>

GstObject* gst_utils_get_root (GstObject * start)
{
  GstObject *tmp;

  if (start == NULL) {
    return NULL;
  }

  tmp = start;

  do {
    start = tmp;
  } while ((tmp = gst_object_get_parent (start)) != NULL);

  return start;
}

GstElement* gst_utils_get_element_from_path (GstElement * root, gchar * path)
{
  if (root == NULL || path == NULL || strlen (path)) {
    return NULL;
  }

  assert (path[0] == '/');

  gchar ** element_names = g_strsplit (path, "/", -1);

  gint i, size;
  GstElement * sp = root;

  for (size = 0; element_names && element_names[size]; size++);

  for (i = 0; i < size; i++) {
    if (GST_IS_BIN (sp)) {
      sp = gst_bin_get_by_name (GST_BIN (sp), element_names[i]);
    } else if (i != size-1) {
      g_strfreev (element_names);
      return NULL;
    }
  }

  g_strfreev (element_names);
  return sp;
}

GstPad* gst_utils_get_pad_from_path (GstElement * root, gchar * pad_path)
{
  if (pad_path == NULL || strlen (pad_path) == 0) {
    return NULL;
  }

  GstPad *pad = NULL;
  gchar *tmp_path = g_strdup (pad_path);
  gchar *pad_name = strrchr(tmp_path, ':');
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
  g_free (pad_name);
  g_free (tmp_path);
  return pad;
}

gchar* gst_utils_get_object_path (GstObject *obj)
{
  GString *path;

  assert (obj != NULL);

  const gchar *init = gst_object_get_parent (obj) == NULL ? NULL : GST_OBJECT_NAME (obj);

  path = g_string_new (init);

  if (GST_IS_PAD (obj)) {
    g_string_prepend_c (path, ':');
  } else if (GST_IS_ELEMENT (obj)) {
    g_string_prepend_c (path, '/');
  } else {
    assert (FALSE); // only GstElement and GstPad allowed
  }

  obj = gst_object_get_parent (obj);

  while (obj != NULL && gst_object_get_parent (obj) != NULL) {
    g_string_prepend (path, GST_OBJECT_NAME (obj));
    g_string_prepend_c (path, '/');
    obj = gst_object_get_parent (obj);
  }

  return g_string_free (path, FALSE);
}
