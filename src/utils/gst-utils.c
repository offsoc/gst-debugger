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

GstElement* gst_utils_get_element_from_path (GstElement * root, const gchar * path)
{
  if (root == NULL || path == NULL) {
    return NULL;
  }

  assert (path[0] == '/');

  if (strlen (path) == 1) {
    return root;
  }

  gchar ** element_names = g_strsplit (path, "/", -1);

  gint i, size;
  GstElement * sp = root;

  for (size = 0; element_names && element_names[size]; size++);

  for (i = 0; i < size; i++) {
    if (strlen (element_names[i]) == 0) {
      continue;
    }
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

GstPad* gst_utils_get_pad_from_path (GstElement * root, const gchar * pad_path)
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

void gst_utils_make_pad_path (GstPad * pad, gchar * buffer, gint max_size)
{
  gchar *pad_name = GST_PAD_NAME (pad);
  gchar *parent_name = (GST_OBJECT_NAME (GST_OBJECT_PARENT (pad)));

  assert ((gint) (strlen (pad_name) + strlen (parent_name) + 1) <= max_size); // todo return required size?

  strcpy (buffer, parent_name);
  strcat (buffer, ":");
  strcat (buffer, pad_name);
}

gboolean gst_utils_check_pad_has_element_parent (GstPad * pad)
{
  GstObject *obj = GST_OBJECT_PARENT (pad);
  if (obj == NULL) {
    return FALSE;
  } else if (GST_IS_ELEMENT (obj)) {
    return TRUE;
  } else if (GST_IS_PAD (obj)) { // internal pad
    obj = GST_OBJECT_PARENT (obj);
    return obj != NULL && GST_IS_ELEMENT (obj);
  }
  return FALSE;
}
