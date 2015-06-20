/*
 * deserializer.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_PROTOCOL_DESERIALIZER_H_
#define SRC_PROTOCOL_DESERIALIZER_H_

#include <gst/gst.h>

GstQuery* gst_query_deserialize (gchar * buffer, gint size);

#endif /* SRC_PROTOCOL_DESERIALIZER_H_ */
