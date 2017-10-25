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

#include "gstdebugservertopology.h"
#include "common/gstdebugger.pb-c.h"
#include <stdio.h>
#include <string.h>
#include "../common/gst-utils.h"

GSList *src_pads;

static GstDebugger__PadTemplate
get_topology_template_object (GstPadTemplate * template)
{
  GstDebugger__PadTemplate tpl = GST_DEBUGGER__PAD_TEMPLATE__INIT;

  tpl.caps = gst_caps_to_string (GST_PAD_TEMPLATE_CAPS (template));
  tpl.direction = GST_PAD_TEMPLATE_DIRECTION (template);
  tpl.presence = GST_PAD_TEMPLATE_PRESENCE (template);
  tpl.name_template = GST_PAD_TEMPLATE_NAME_TEMPLATE (template);

  return tpl;
}

static void
send_object (GstObject * object, GstDebugger__Action action,
    GstDebugserverTcp * server, TcpClient * client)
{
  GstDebugger__GStreamerData info = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__TopologyInfo topology = GST_DEBUGGER__TOPOLOGY_INFO__INIT;
  GstDebugger__TopologyElement element_tp =
      GST_DEBUGGER__TOPOLOGY_ELEMENT__INIT;
  GstDebugger__TopologyPad pad_tp = GST_DEBUGGER__TOPOLOGY_PAD__INIT;
  GstDebugger__PadTemplate template = GST_DEBUGGER__PAD_TEMPLATE__INIT;

  info.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_TOPOLOGY_INFO;
  topology.action = action;

  if (GST_IS_ELEMENT (object)) {
    GstElementFactory *factory =
        gst_element_get_factory (GST_ELEMENT_CAST (object));
    element_tp.type_name = (gchar *) g_type_name (G_OBJECT_TYPE (object));
    element_tp.path = gst_utils_get_object_path (object);
    element_tp.is_bin = GST_IS_BIN (object);
    if (factory != NULL) {
      element_tp.factory_name =
          gst_plugin_feature_get_name (gst_element_get_factory (GST_ELEMENT_CAST
              (object)));
    }
    topology.element = &element_tp;
    topology.topology_type_case =
        GST_DEBUGGER__TOPOLOGY_INFO__TOPOLOGY_TYPE_ELEMENT;
  } else if (GST_IS_PAD (object)) {
    GstPad *pad = GST_PAD (object);
    pad_tp.path = gst_utils_get_object_path (object);
    pad_tp.is_ghostpad = GST_IS_GHOST_PAD (pad);
    pad_tp.presence =
        gst_pad_get_pad_template (pad) ?
        GST_PAD_TEMPLATE_PRESENCE (gst_pad_get_pad_template (pad)) : 0;
    pad_tp.direction = GST_PAD_DIRECTION (pad);
    if (GST_PAD_PAD_TEMPLATE (pad)) {
      template = get_topology_template_object (GST_PAD_PAD_TEMPLATE (pad));
      pad_tp.template_ = &template;
    }
    topology.pad = &pad_tp;
    topology.topology_type_case =
        GST_DEBUGGER__TOPOLOGY_INFO__TOPOLOGY_TYPE_PAD;
  } else {
    assert (FALSE);
  }
  info.topology_info = &topology;

  gst_debugserver_tcp_send_packet (server, client, &info);
  g_free (template.caps);
  g_free (pad_tp.path);
  g_free (element_tp.path);
}

static void
send_link (GstPad * src_pad, GstPad * sink_pad, GstDebugger__Action action,
    GstDebugserverTcp * server, TcpClient * client)
{
  GstDebugger__GStreamerData info = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__TopologyInfo topology = GST_DEBUGGER__TOPOLOGY_INFO__INIT;
  GstDebugger__TopologyLink link_tp = GST_DEBUGGER__TOPOLOGY_LINK__INIT;

  if (!gst_utils_check_pad_has_element_parent (src_pad)
      || !gst_utils_check_pad_has_element_parent (sink_pad)) {
    return;
  }

  info.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_TOPOLOGY_INFO;
  topology.action = action;
  topology.topology_type_case = GST_DEBUGGER__TOPOLOGY_INFO__TOPOLOGY_TYPE_LINK;

  if (GST_IS_PROXY_PAD (src_pad) && !GST_IS_GHOST_PAD (src_pad)) {
    return;
  }

  link_tp.src_pad = gst_utils_get_object_path (GST_OBJECT_CAST (src_pad));
  if (GST_IS_PROXY_PAD (sink_pad)) {
    if (GST_IS_GHOST_PAD (sink_pad)) {
      GstPad *internal =
          gst_pad_get_peer (GST_PAD_CAST (gst_proxy_pad_get_internal
              (GST_PROXY_PAD (sink_pad))));
      if (internal != NULL) {
        send_link (sink_pad, internal, action, server, client);
      }
    } else {
      sink_pad =
          GST_PAD_CAST (gst_proxy_pad_get_internal (GST_PROXY_PAD (sink_pad)));
    }
  }

  link_tp.sink_pad = gst_utils_get_object_path (GST_OBJECT_CAST (sink_pad));
  topology.link = &link_tp;
  topology.topology_type_case = GST_DEBUGGER__TOPOLOGY_INFO__TOPOLOGY_TYPE_LINK;
  info.topology_info = &topology;

  gst_debugserver_tcp_send_packet (server, client, &info);

  g_free (link_tp.src_pad);
  g_free (link_tp.sink_pad);
}

static void
send_element_pads (GstElement * element, GstDebugserverTcp * server,
    TcpClient * client)
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
        if (gst_pad_get_direction (pad) == GST_PAD_SRC
            && gst_pad_get_peer (pad)) {
          src_pads = g_slist_append (src_pads, pad);
        }
        send_object (GST_OBJECT (pad), GST_DEBUGGER__ACTION__ADD, server,
            client);
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

static void
gst_debugserver_topology_send_element (GstElement * element,
    GstDebugserverTcp * server, TcpClient * client)
{
  GstIterator *element_it;
  gboolean done;

  if (GST_ELEMENT_PARENT (element) != NULL) {
    send_object (GST_OBJECT (element), GST_DEBUGGER__ACTION__ADD, server,
        client);
  }

  send_element_pads (element, server, client);

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
        gst_debugserver_topology_send_element (element, server, client);
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

void
gst_debugserver_topology_send_entire_topology (GstBin * bin,
    GstDebugserverTcp * server, TcpClient * client)
{
  src_pads = NULL;
  gst_debugserver_topology_send_element (GST_ELEMENT (bin), server, client);
  GSList *tmp_list = src_pads;
  while (tmp_list != NULL) {
    GstPad *pad = (GstPad *) tmp_list->data;
    send_link (pad, gst_pad_get_peer (pad), GST_DEBUGGER__ACTION__ADD, server,
        client);
    tmp_list = tmp_list->next;
  }

  g_slist_free (src_pads);
}

void
gst_debugserver_topology_send_pad_link (GstPad * src, GstPad * sink,
    gboolean link, GstDebugserverTcp * server, TcpClient * client)
{
  send_link (src, sink,
      link ? GST_DEBUGGER__ACTION__ADD : GST_DEBUGGER__ACTION__REMOVE, server,
      client);
}

void
gst_debugserver_topology_send_element_in_bin (GstBin * bin,
    GstElement * element, gboolean add, GstDebugserverTcp * server,
    TcpClient * client)
{
  send_object (GST_OBJECT_CAST (element),
      add ? GST_DEBUGGER__ACTION__ADD : GST_DEBUGGER__ACTION__REMOVE, server,
      client);
}

void
gst_debugserver_topology_send_pad_in_element (GstElement * element,
    GstPad * pad, gboolean add, GstDebugserverTcp * server, TcpClient * client)
{
  if (GST_OBJECT_PARENT (element) == NULL) {
    return;
  }

  send_object (GST_OBJECT_CAST (pad),
      add ? GST_DEBUGGER__ACTION__ADD : GST_DEBUGGER__ACTION__REMOVE, server,
      client);
}
