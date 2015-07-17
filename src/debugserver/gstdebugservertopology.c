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

static void send_object (GstObject *object, Topology__Action action, GstDebugserverTcp * server)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  Topology topology = TOPOLOGY__INIT;
  gint size;
  gchar buffer[1024];
  TopologyElement element_tp = TOPOLOGY_ELEMENT__INIT;
  TopologyPad pad_tp = TOPOLOGY_PAD__INIT;

  info.info_type = GSTREAMER_INFO__INFO_TYPE__TOPOLOGY;
  topology.action = action;

  if (GST_IS_ELEMENT (object)) {
    element_tp.type_name = g_strdup (g_type_name (G_OBJECT_TYPE (object)));
    element_tp.path = gst_utils_get_object_path (object);
    element_tp.is_bin = GST_IS_BIN (object);
    topology.element = &element_tp;
    topology.type = TOPOLOGY__OBJECT_TYPE__ELEMENT;
  } else if (GST_IS_PAD (object)) {
    GstPad *pad = GST_PAD (object);
    pad_tp.path = gst_utils_get_object_path (object);
    pad_tp.tpl_name = g_strdup (
    		gst_pad_get_pad_template (pad) ? GST_PAD_TEMPLATE_NAME_TEMPLATE (gst_pad_get_pad_template (pad)) : "");
    pad_tp.is_ghostpad = GST_IS_GHOST_PAD (pad);
    pad_tp.presence = gst_pad_get_pad_template (pad) ? GST_PAD_TEMPLATE_PRESENCE (gst_pad_get_pad_template (pad)) : 0;
    pad_tp.direction = GST_PAD_DIRECTION (pad);
    topology.pad = &pad_tp;
    topology.type = TOPOLOGY__OBJECT_TYPE__PAD;
  } else {
    assert (FALSE);
  }
  info.topology = &topology;
  size = gstreamer_info__get_packed_size (&info);
  assert(size <= 1024);
  gstreamer_info__pack (&info, (guint8*)buffer);

  gst_debugserver_tcp_send_packet_to_all_clients (server, buffer, size);
}

static void send_link (GstPad *src_pad, GstPad *sink_pad, Topology__Action action, GstDebugserverTcp *server)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  Topology topology = TOPOLOGY__INIT;
  gint size;
  gchar buffer[1024];
  TopologyLink link_tp = TOPOLOGY_LINK__INIT;

  if (!gst_utils_check_pad_has_element_parent (src_pad) || !gst_utils_check_pad_has_element_parent (sink_pad)) {
    return;
  }

  info.info_type = GSTREAMER_INFO__INFO_TYPE__TOPOLOGY;
  topology.action = action;
  topology.type = TOPOLOGY__OBJECT_TYPE__LINK;

  link_tp.src_pad_path = gst_utils_get_object_path (GST_OBJECT_CAST (src_pad));
  if (GST_IS_PROXY_PAD (sink_pad)) {
    if (GST_IS_GHOST_PAD (sink_pad)) {
      send_link (sink_pad, gst_pad_get_peer (GST_PAD_CAST (gst_proxy_pad_get_internal (GST_PROXY_PAD (sink_pad)))), action, server);
    } else {
      sink_pad = GST_PAD_CAST (gst_proxy_pad_get_internal (GST_PROXY_PAD (sink_pad)));
    }
  }
  link_tp.sink_pad_path = gst_utils_get_object_path (GST_OBJECT_CAST (sink_pad));
  topology.link = &link_tp;
  topology.type = TOPOLOGY__OBJECT_TYPE__LINK;
  info.topology = &topology;
  size = gstreamer_info__get_packed_size (&info);
  assert(size <= 1024);
  gstreamer_info__pack (&info, (guint8*)buffer);
  gst_debugserver_tcp_send_packet_to_all_clients (server, buffer, size);
}

static void send_element_pads (GstElement * element, GstDebugserverTcp *server)
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
          src_pads = g_slist_append (src_pads, pad);
        }
        send_object (GST_OBJECT (pad), TOPOLOGY__ACTION__ADD, server);
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

static void gst_debugserver_topology_send_element (GstElement * element, GstDebugserverTcp *server)
{
  GstIterator *element_it;
  gboolean done;

  if (GST_ELEMENT_PARENT (element) != NULL) {
    send_object (GST_OBJECT (element), TOPOLOGY__ACTION__ADD, server);
  }

  send_element_pads (element, server);

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
        gst_debugserver_topology_send_element (element, server);
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
  g_value_unset (&item);
  gst_iterator_free (element_it);
}

void gst_debugserver_topology_send_entire_topology (GstBin *bin, GstDebugserverTcp * server)
{
  src_pads = NULL;
  gst_debugserver_topology_send_element (GST_ELEMENT (bin), server);
  GSList *tmp_list = src_pads;
  while (tmp_list != NULL) {
    GstPad *pad = (GstPad*)tmp_list->data;
    send_link (pad, gst_pad_get_peer (pad), TOPOLOGY__ACTION__ADD, server);
    tmp_list = tmp_list->next;
  }

  g_slist_free (src_pads);
}

void gst_debugserver_topology_send_pad_link (GstPad * src, GstPad * sink, gboolean link, GstDebugserverTcp * server)
{
  GstElement *parent_element = GST_PAD_PARENT (src);
  if (parent_element == NULL) { // todo try to comment it
    return;
  }
  if (GST_OBJECT_PARENT (parent_element) == NULL) { // todo try to comment it
    return;
  }
  send_link (src, sink, link ? TOPOLOGY__ACTION__ADD : TOPOLOGY__ACTION__REMOVE, server);
}

void gst_debugserver_topology_send_element_in_bin (GstBin * bin, GstElement * element, gboolean add, GstDebugserverTcp * server)
{
  send_object (GST_OBJECT_CAST (element), add ? TOPOLOGY__ACTION__ADD : TOPOLOGY__ACTION__REMOVE, server);
}

void gst_debugserver_topology_send_pad_in_element (GstElement * element, GstPad * pad, gboolean add, GstDebugserverTcp * server)
{
  if (GST_OBJECT_PARENT (element) == NULL) {
    return;
  }

  send_object (GST_OBJECT_CAST (pad), add ? TOPOLOGY__ACTION__ADD : TOPOLOGY__ACTION__REMOVE, server);
}

