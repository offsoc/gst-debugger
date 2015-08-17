/*
 * serializer.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_SERIALIZATION_SERIALIZER_H_
#define SRC_GST_SERIALIZATION_SERIALIZER_H_

#include "common.h"

#include <gst/gst.h>

G_BEGIN_DECLS

gint gst_query_serialize(GstQuery * query, gchar * buffer, gint size);

gint gst_event_serialize(GstEvent * event, gchar * buffer, gint size);

gint gst_message_serialize(GstMessage * message, gchar * buffer, gint size);

gint gst_buffer_serialize(GstBuffer * gstbuffer, gchar * buffer, gint size);

gchar * g_value_serialize (GValue * value, GType * type, InternalGType * internal_type);

G_END_DECLS

#endif /* SRC_GST_SERIALIZATION_SERIALIZER_H_ */
