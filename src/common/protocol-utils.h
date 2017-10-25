/*
 * protocol-utils.h
 *
 *  Created on: Jun 9, 2015
 *      Author: mkolny
 */

#ifndef PROTOCOL_UTILS_H_
#define PROTOCOL_UTILS_H_

#include <gio/gio.h>

G_BEGIN_DECLS

gboolean gst_debugger_protocol_utils_read_requested_size (GInputStream *
    istream, gint requested_size, gchar * buffer, GCancellable * cancel);

gint gst_debugger_protocol_utils_read_header (GInputStream * istream,
    GCancellable * cancel);

GError *gst_debugger_protocol_write_header (GOutputStream * ostream,
    gint64 value);

G_END_DECLS

#endif /* PROTOCOL_UTILS_H_ */
