/*
 * serializer.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_SERIALIZATION_SERIALIZER_H_
#define SRC_GST_SERIALIZATION_SERIALIZER_H_

#include <gst/gst.h>

G_BEGIN_DECLS

gint gst_query_serialize(GstQuery * query, gchar * buffer, gint size);

gint gst_event_serialize(GstEvent * event, gchar * buffer, gint size);

gint gst_message_serialize(GstMessage * message, gchar * buffer, gint size);

G_END_DECLS

#endif /* SRC_GST_SERIALIZATION_SERIALIZER_H_ */
