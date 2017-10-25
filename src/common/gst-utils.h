#ifndef SRC_COMMON_GST_UTILS_H_
#define SRC_COMMON_GST_UTILS_H_

#include <gst/gst.h>

G_BEGIN_DECLS

typedef struct
{
  gchar *type_name;
} GstUnknownType;

GType gst_unknown_type_get_type (void);

GstUnknownType *gst_unknown_type_new (const gchar * type_name);

GstElement *gst_utils_get_element_from_path (GstElement * root,
    const gchar * path);

GstPad *gst_utils_get_pad_from_path (GstElement * root, const gchar * pad_path);

gchar *gst_utils_get_object_path (GstObject * obj);

gboolean gst_utils_check_pad_has_element_parent (GstPad * pad);

GType gst_utils_get_virtual_enum_type (void);

GType gst_utils_get_virtual_flags_type (void);

G_END_DECLS

#endif /* SRC_COMMON_GST_UTILS_H_ */
