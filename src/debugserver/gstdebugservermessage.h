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

#ifndef __GST_DEBUGSERVER_MESSAGE_H__
#define __GST_DEBUGSERVER_MESSAGE_H__

#include "protocol/gstdebugger.pb-c.h"

#include <gst/gst.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _GstDebugserverMessage GstDebugserverMessage;

struct _GstDebugserverMessage {
  GHashTable *clients;
};

GstDebugserverMessage * gst_debugserver_message_new (void);
void gst_debugserver_message_free (GstDebugserverMessage * msg);

gboolean gst_debugserver_message_add_watch (GstDebugserverMessage * msg,
  GstMessageType msg_type, gpointer client_info);

gboolean gst_debugserver_message_remove_watch (GstDebugserverMessage * msg,
  GstMessageType msg_type, gpointer client_info);

gboolean gst_debugserver_message_set_watch (GstDebugserverMessage * msg,
  gboolean enable, GstMessageType msg_type, gpointer client_info);

void gst_debugserver_message_remove_client (GstDebugserverMessage * msg,
  gpointer client_info);

GSList* gst_debugserver_message_get_clients (GstDebugserverMessage * msg,
  GstMessageType msg_type);

gint gst_debugserver_message_prepare_buffer (GstMessage * gst_msg,
  gchar * buffer, gint max_size);

gint gst_debugserver_message_prepare_confirmation_buffer (MessageWatch *msg_watch,
  gchar * buffer, gint max_size);

void gst_debugserver_message_clean (GstDebugserverMessage * msg);

G_END_DECLS

#endif /* __GST_DEBUGSERVER_MESSAGE_H__ */
