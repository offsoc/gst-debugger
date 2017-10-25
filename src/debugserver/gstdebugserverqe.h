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
#include "gstdebugserverhooks.h"

G_BEGIN_DECLS

typedef struct _GstDebugserverQE GstDebugserverQE;

struct _GstDebugserverQE
{
  GstDebugserverHooks hooks;
};

GstDebugserverQE *gst_debugserver_qe_new (void);

void gst_debugserver_qe_free (GstDebugserverQE * qe);

gboolean gst_debugserver_qe_set_hook (GstDebugserverQE * qe, gboolean enable,
    gint qe_type, GstPad * pad, gchar * pad_path, TcpClient * client);

void gst_debugserver_qe_send_qe (GstDebugserverQE * qe,
    GstDebugserverTcp * tcp_server, GstPad * pad, GstMiniObject * obj);

void gst_debugserver_qe_remove_client (GstDebugserverQE * evt,
    TcpClient * client);

G_END_DECLS

#endif /* __GST_DEBUGSERVER_EVENT_H__ */
