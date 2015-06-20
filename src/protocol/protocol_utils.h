/*
 * protocol_utils.h
 *
 *  Created on: Jun 9, 2015
 *      Author: mkolny
 */

#ifndef PROTOCOL_UTILS_H_
#define PROTOCOL_UTILS_H_

#include <gio/gio.h>

gboolean gst_debugger_protocol_utils_read_requested_size (GInputStream * istream, gint requested_size, gchar * buffer);

gint gst_debugger_protocol_utils_read_header (GInputStream * istream);

void gst_debugger_protocol_utils_serialize_integer64 (gint64 value, gchar * buffer, gint size);

gint64 gst_debugger_protocol_utils_deserialize_integer64 (gchar * buffer, gint size);

void gst_debugger_protocol_utils_serialize_uinteger64 (guint64 value, gchar * buffer, gint size);

guint64 gst_debugger_protocol_utils_deserialize_uinteger64 (gchar * buffer, gint size);

#endif /* PROTOCOL_UTILS_H_ */
