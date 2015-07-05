/* GStreamer
 * Copyright (C) 2015 Marcin Kolny <marcin.kolny@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GST_DEBUGSERVER_BUFFER_H__
#define __GST_DEBUGSERVER_BUFFER_H__

#include "protocol/gstdebugger.pb-c.h"

#include <gst/gst.h>

G_BEGIN_DECLS

typedef struct _GstDebugserverBuffer GstDebugserverBuffer;

struct _GstDebugserverBuffer {
  GHashTable *clients;
};

GstDebugserverBuffer * gst_debugserver_buffer_new (void);
void gst_debugserver_buffer_free (GstDebugserverBuffer * buf);

gboolean gst_debugserver_buffer_add_watch (GstDebugserverBuffer * buf,
  GstPad * pad, gpointer client_info);

gboolean gst_debugserver_buffer_remove_watch (GstDebugserverBuffer * buf,
  GstPad * pad, gpointer client_info);

gboolean gst_debugserver_buffer_set_watch (GstDebugserverBuffer * buf,
  gboolean enable, GstPad * pad, gpointer client_info);

GList* gst_debugserver_buffer_get_clients (GstDebugserverBuffer * buf,
  GstPad * pad);


G_END_DECLS

#endif /* __GST_DEBUGSERVER_BUFFER_H__ */
