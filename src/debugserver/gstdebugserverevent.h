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

#ifndef __GST_DEBUGSERVER_EVENT_H__
#define __GST_DEBUGSERVER_EVENT_H__

#include <gst/gst.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _GstDebugserverEvent GstDebugserverEvent;

typedef struct _EventWatch {
  gint event_type;
  GstPad * pad;
} EventWatch;

struct _GstDebugserverEvent {
  GHashTable * watches;
};

GstDebugserverEvent * gst_debugserver_event_new (void);

void gst_debugserver_event_free (GstDebugserverEvent * evt);

gint gst_debugserver_event_prepare_confirmation_buffer (gchar * pad_path, gint event_type,
  gboolean toggle, gchar * buffer, gint max_size);

gint gst_debugserver_event_prepare_buffer (GstEvent * event, gchar * buffer, gint max_size);

GSList* gst_debugserver_event_get_clients (GstDebugserverEvent * evt, GstPad * pad,
  GstEvent * event);

gboolean gst_debugserver_event_set_watch (GstDebugserverEvent * evt, gboolean enable,
  GstPad * pad, gint event_type, gpointer client_info);

G_END_DECLS

#endif /* __GST_DEBUGSERVER_EVENT_H__ */
