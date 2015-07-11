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

#include "utils/gst-utils.h"
#include "gstdebugservertopology.h"
#include "protocol/gstdebugger.pb-c.h"
#include <stdio.h>
#include <string.h>

GSList *src_pads;

typedef struct {
  GstPad *pad;
  gchar *bin;
} PadLinkInfo;

static void send_object (GstObject *object, gchar *bin_path, GstDebugserverTcp *server)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  Topology topology = TOPOLOGY__INIT;
  gint size;
  gchar buffer[1024];
  TopologyElement element_tp = TOPOLOGY_ELEMENT__INIT;
  TopologyPad pad_tp = TOPOLOGY_PAD__INIT;

  info.info_type = GSTREAMER_INFO__INFO_TYPE__TOPOLOGY;
  topology.action = TOPOLOGY__ACTION__ADD;
  topology.bin_path = g_strdup (bin_path);

  if (GST_IS_ELEMENT (object)) {
    GstElement *element = GST_ELEMENT (object);
    element_tp.name = g_strdup (GST_ELEMENT_NAME (element));
    element_tp.factory = g_strdup (gst_plugin_feature_get_name (GST_PLUGIN_FEATURE (gst_element_get_factory (element))));
    topology.element = &element_tp;
    topology.type = TOPOLOGY__OBJECT_TYPE__ELEMENT;
  } else if (GST_IS_PAD (object)) {
    GstPad *pad = GST_PAD (object);
    pad_tp.element = g_strdup (GST_ELEMENT_NAME (GST_PAD_PARENT (pad)));
    pad_tp.name = GST_PAD_NAME (pad);
    pad_tp.tpl_name = g_strdup (GST_PAD_TEMPLATE_NAME_TEMPLATE (gst_pad_get_pad_template (pad)));
    pad_tp.is_ghostpad = GST_IS_GHOST_PAD (pad);
    topology.pad = &pad_tp;
    topology.type = TOPOLOGY__OBJECT_TYPE__PAD;
  } else {
    assert (FALSE);
  }
  info.topology = &topology;
  size = gstreamer_info__get_packed_size (&info);
  assert(size <= 1024);
  gstreamer_info__pack (&info, (guint8*)buffer);
  gst_debugserver_tcp_send_packet (server, ((TcpClient*) (server->clients->data))->connection, buffer, size);
}

static void send_link (GstPad *src_pad, gchar *bin_path, GstDebugserverTcp *server)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  Topology topology = TOPOLOGY__INIT;
  gint size;
  gchar buffer[1024];
  TopologyLink link_tp = TOPOLOGY_LINK__INIT;
  gchar tmpbuff[128];
  info.info_type = GSTREAMER_INFO__INFO_TYPE__TOPOLOGY;
  topology.action = TOPOLOGY__ACTION__ADD;
  topology.type = TOPOLOGY__OBJECT_TYPE__LINK;
  topology.bin_path = g_strdup (bin_path);

  strcpy (tmpbuff, GST_OBJECT_NAME (GST_OBJECT_PARENT (src_pad)));
  strcat (tmpbuff, ":");
  strcat (tmpbuff, GST_PAD_NAME (src_pad));
  link_tp.src_pad_path = g_strdup (tmpbuff);
  src_pad = gst_pad_get_peer (src_pad);
  memset(tmpbuff, 0, 128);
  strcpy (tmpbuff, GST_OBJECT_NAME (GST_OBJECT_PARENT (src_pad)));
  strcat (tmpbuff, ":");
  strcat (tmpbuff, GST_PAD_NAME (src_pad));
  link_tp.sink_pad_path = g_strdup (tmpbuff);
  topology.link = &link_tp;
  topology.type = TOPOLOGY__OBJECT_TYPE__LINK;
  info.topology = &topology;
  size = gstreamer_info__get_packed_size (&info);
  assert(size <= 1024);
  gstreamer_info__pack (&info, (guint8*)buffer);
  gst_debugserver_tcp_send_packet (server, ((TcpClient*) (server->clients->data))->connection, buffer, size);
}

static void send_element_pads (GstElement * element, GString *path, GstDebugserverTcp *server)
{
  gboolean done;
  GstPad *pad;
  GValue item = G_VALUE_INIT;
  done = FALSE;
  GstIterator *pad_it = gst_element_iterate_pads (element);

  while (!done) {
    switch (gst_iterator_next (pad_it, &item)) {
      case GST_ITERATOR_OK:
        pad = g_value_get_object (&item);
        if (gst_pad_get_direction (pad) == GST_PAD_SRC && gst_pad_get_peer (pad)) {
          PadLinkInfo *nfo = (PadLinkInfo*) g_malloc (sizeof (PadLinkInfo));
          nfo->pad = pad;
          nfo->bin = g_strdup (path->str); // todo memleak
          src_pads = g_slist_append (src_pads, nfo);
        }
        send_object (GST_OBJECT (pad), path->str, server);
        g_value_reset (&item);
        break;
      case GST_ITERATOR_RESYNC:
        gst_iterator_resync (pad_it);
        break;
      case GST_ITERATOR_ERROR:
      case GST_ITERATOR_DONE:
        done = TRUE;
        break;
    }
  }
  g_value_unset (&item);
  gst_iterator_free (pad_it);
}

static void gst_debugserver_topology_send_element (GstElement * element, GString *path, GstDebugserverTcp *server)
{
  GstIterator *element_it;
  gboolean done;
  GString *intern_path = g_string_new (path->str);

  if (GST_ELEMENT_PARENT (element) != NULL) {
    send_object (GST_OBJECT (element), path->str, server);
    if (GST_ELEMENT_PARENT (GST_ELEMENT_PARENT (element)) != NULL) {
      g_string_append_c (intern_path, '/');
    }
    g_string_append (intern_path, GST_ELEMENT_NAME (element));
  } else {
    g_string_append_c (intern_path, '/');
  }

  send_element_pads (element, path, server);

  if (!GST_IS_BIN (element)) {
    return;
  }

  element_it = gst_bin_iterate_elements (GST_BIN (element));
  GValue item = G_VALUE_INIT;
  done = FALSE;
  while (!done) {
    switch (gst_iterator_next (element_it, &item)) {
      case GST_ITERATOR_OK:
        element = g_value_get_object (&item);
        gst_debugserver_topology_send_element (element, intern_path, server);
        g_value_reset (&item);
        break;
      case GST_ITERATOR_RESYNC:
        gst_iterator_resync (element_it);
        break;
      case GST_ITERATOR_ERROR:
      case GST_ITERATOR_DONE:
        done = TRUE;
        break;
    }
  }
  g_string_free (intern_path, TRUE);
  g_value_unset (&item);
  gst_iterator_free (element_it);
}

void gst_debugserver_topology_send_entire_topology (GstBin *bin, GstDebugserverTcp *client)
{
  GString *path = g_string_new (NULL);
  src_pads = NULL;
  gst_debugserver_topology_send_element (GST_ELEMENT (bin), path, client);
  g_string_free (path, TRUE);
  GSList *tmp_list = src_pads;
  while (tmp_list != NULL) {
    PadLinkInfo *nfo = (PadLinkInfo*)tmp_list->data;
    GstPad *p = nfo->pad;
    send_link (p, nfo->bin, client);
    tmp_list = tmp_list->next;
  }

  g_slist_free_full (src_pads, g_free);
}
