/*
 * deserializer.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_PROTOCOL_DESERIALIZER_H_
#define SRC_PROTOCOL_DESERIALIZER_H_

#include <gst/gst.h>

G_BEGIN_DECLS

GstQuery* gst_query_deserialize (const gchar * buffer, gint size);

GstEvent* gst_event_deserialize (const gchar * buffer, gint size);

GstMessage* gst_message_deserialize (const gchar * buffer, gint size);

G_END_DECLS

#endif /* SRC_PROTOCOL_DESERIALIZER_H_ */
