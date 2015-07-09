#ifndef __GST_UTILS_H__
#define __GST_UTILS_H__

#include <gst/gst.h>

GstPad* gst_utils_find_pad (GstPipeline * start, gchar * pad_path);

#endif /* __GST_UTILS_H__ */
