#ifndef __GST_UTILS_H__
#define __GST_UTILS_H__

#include <gst/gst.h>

G_BEGIN_DECLS

GstObject* gst_utils_get_root (GstObject * start);

GstElement* gst_utils_get_element_from_path (GstElement * root, gchar * path);

GstPad* gst_utils_get_pad_from_path (GstElement * root, gchar * pad_path);

G_END_DECLS

#endif /* __GST_UTILS_H__ */
