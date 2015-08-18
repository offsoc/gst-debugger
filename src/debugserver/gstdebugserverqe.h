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

#include "common/gstdebugger.pb-c.h"

#include <gst/gst.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _GstDebugserverQE GstDebugserverQE;

typedef struct _QEWatch {
  gint qe_type;
  GstPad * pad;
} QEWatch;

struct _GstDebugserverQE {
  GHashTable * watches;
};

GstDebugserverQE * gst_debugserver_qe_new (void);

void gst_debugserver_qe_free (GstDebugserverQE * qe);

gint gst_debugserver_qeb_prepare_confirmation_buffer (gchar * pad_path, gint qe_type,
  gboolean toggle, gchar * buffer, gint max_size, PadWatch__WatchType type);

gboolean gst_debugserver_qe_set_watch (GstDebugserverQE * qe, gboolean enable,
  GstPad * pad, gint qe_type, gpointer client_info);

GSList* gst_debugserver_qe_get_clients (GstDebugserverQE * qe, GstPad * pad,
  gint type);

gint gst_debugserver_qebm_prepare_buffer (GstMiniObject * miniobj, gchar *pad_path,
  gchar * buffer, gint max_size);

void gst_debugserver_qe_clean (GstDebugserverQE * qe);

void gst_debugserver_qe_remove_client (GstDebugserverQE * evt, gpointer client_info);

G_END_DECLS

#endif /* __GST_DEBUGSERVER_EVENT_H__ */
