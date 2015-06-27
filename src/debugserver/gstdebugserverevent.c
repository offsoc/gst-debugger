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

#include "gstdebugserverevent.h"
#include "protocol/serializer.h"
#include "protocol/gstdebugger.pb-c.h"

#include <string.h>

GstDebugserverEvent * gst_debugserver_event_new (void)
{
  GstDebugserverEvent *evt = (GstDebugserverEvent*)g_malloc (sizeof(GstDebugserverEvent));
  evt->clients = NULL;

  return evt;
}

void gst_debugserver_event_free (GstDebugserverEvent * evt)
{
  g_slist_free (evt->clients);
  g_free (evt);
}

gint gst_debugserver_event_prepare_buffer (GstEvent * event, gchar * buffer, gint max_size)
{
  gchar buff[1024];
  // todo verify max_size
  gint size = gst_event_serialize (event, buff, max_size);
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  GstreamerEvent evt = GSTREAMER_EVENT__INIT;
  evt.payload.len = size;
  evt.payload.data = (uint8_t*)g_malloc (size);
  memcpy (evt.payload.data, buff, size);
  info.info_type = GSTREAMER_INFO__INFO_TYPE__EVENT;
  info.event = &evt;

  assert(size <= max_size);
  gstreamer_info__pack (&info, (guint8*)buffer);
  return size;
}

GSList* gst_debugserver_event_get_clients (GstDebugserverEvent * evt)
{
  return evt->clients;
}

void gst_debugserver_event_set_watch (GstDebugserverEvent * evt, gboolean enable,
  gpointer client_info)
{
  if (enable == TRUE) {
    if (g_slist_find (evt->clients, client_info) == NULL) {
      evt->clients = g_slist_append (evt->clients, client_info);
    }
  } else {
    evt->clients = g_slist_remove (evt->clients, client_info);
  }
}
