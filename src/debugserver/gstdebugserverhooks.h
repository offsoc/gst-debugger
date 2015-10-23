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

#ifndef __GST_DEBUGSERVER_HOOKS_H__
#define __GST_DEBUGSERVER_HOOKS_H__

#include "gstdebugservertcp.h"

#include <glib.h>

G_BEGIN_DECLS

typedef gboolean (*OkFunction)(GstDebugger__GStreamerData*, gpointer);

typedef struct {
  GHashTable *clients;
  GMutex mutex;
  OkFunction ok_function;
  GCompareFunc cmp_function;
} GstDebugserverHooks;

void gst_debugserver_hooks_init (GstDebugserverHooks * hooks, OkFunction ok_function, GDestroyNotify hash_destroy, GCompareFunc cmp_func);

void gst_debugserver_hooks_deinit (GstDebugserverHooks * hooks);

void gst_debugserver_hooks_clean (GstDebugserverHooks * hooks);

gboolean gst_debugserver_hooks_add_hook (GstDebugserverHooks * hooks,
  gpointer data, TcpClient * client);

gboolean gst_debugserver_hooks_remove_hook (GstDebugserverHooks * hooks,
  gpointer data, TcpClient * client);

void gst_debugserver_hooks_remove_client (GstDebugserverHooks * hooks, TcpClient * client);

void gst_debugserver_hooks_send_data (GstDebugserverHooks * hooks, GstDebugserverTcp * tcp_server, GstDebugger__GStreamerData * gst_data);

G_END_DECLS

#endif
