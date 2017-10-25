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

gchar * g_value_serialize (GValue * value, GType * type,
    InternalGType * internal_type);

void g_value_deserialize (GValue * value, GType type,
    InternalGType internal_type, const gchar * data, gint len);

G_END_DECLS

#endif /* SRC_GST_SERIALIZATION_SERIALIZER_H_ */
