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

#include "gstdebugserverlog.h"
#include "common/gstdebugger.pb-c.h"

#include <string.h>

typedef struct {
  GstDebugLevel level;
  gchar * category;
} DebugHook;

static DebugHook * debug_hook_new (GstDebugLevel level, const gchar * category)
{
  DebugHook * hook = (DebugHook *) g_malloc (sizeof (DebugHook));

  hook->level = level;
  hook->category = g_strdup (category);

  return hook;
}

static void debug_hook_free (DebugHook * hook)
{
  g_free (hook->category);
  g_free (hook);
}

static void debug_hook_list_free (gpointer ptr)
{
  g_slist_free_full (ptr, (GDestroyNotify) debug_hook_free);
}

static gint debug_hook_compare (gconstpointer p1, gconstpointer p2)
{
  const DebugHook *w1 = p1, *w2 = p2;

  if (w1->level >= w2->level && (w1->category == NULL ||
      g_strcmp0 (w1->category, w2->category) == 0)) {
    return 0;
  } else {
    return 1;
  }
}

static gboolean gst_debugserver_log_ok (GstDebugger__GStreamerData* original, gpointer new_ptr)
{
  GstDebugger__LogInfo* info = original->log_info;
  GSList *list = new_ptr;
  DebugHook hook = { info->level, info->category };

  return g_slist_find_custom (list, &hook, debug_hook_compare) != NULL;
}

GstDebugserverLog * gst_debugserver_log_new (void)
{
  GstDebugserverLog *log = (GstDebugserverLog*)g_malloc (sizeof(GstDebugserverLog));
  gst_debugserver_hooks_init (&log->hooks, gst_debugserver_log_ok, (GDestroyNotify) debug_hook_list_free, debug_hook_compare);

  return log;
}

void gst_debugserver_log_free (GstDebugserverLog * log)
{
  gst_debugserver_hooks_deinit (&log->hooks);
  g_free (log);
}

static gboolean gst_debugserver_log_add_hook (GstDebugserverLog * log, gint level,
  const gchar * category, TcpClient * client)
{
  DebugHook *w = debug_hook_new (level, category);
  if (gst_debugserver_hooks_add_hook (&log->hooks, w, client) == TRUE) {
    return TRUE;
  } else {
    debug_hook_free (w);
    return FALSE;
  }
}

static gboolean gst_debugserver_log_remove_hook (GstDebugserverLog * log,
  gint level, const gchar * category, TcpClient * client)
{
  DebugHook w = { level, (gchar*)category };

  return gst_debugserver_hooks_remove_hook (&log->hooks, &w, client);
}

gboolean gst_debugserver_log_set_hook (GstDebugserverLog * log, gboolean enable,
  gint level, const gchar * category, TcpClient * client)
{
  if (enable) {
    return gst_debugserver_log_add_hook (log, level, category, client);
  } else {
    return gst_debugserver_log_remove_hook (log, level, category, client);
  }
}

void gst_debugserver_log_send_log (GstDebugserverLog * log, GstDebugserverTcp * tcp_server,
  GstDebugCategory * category, GstDebugLevel level, const gchar * file, const gchar * function,
  gint line, GObject * object, GstDebugMessage * message)
{
  GstDebugger__GStreamerData gst_data = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__LogInfo log_info = GST_DEBUGGER__LOG_INFO__INIT;

  log_info.level = (gint)level;
  log_info.category = (gchar*) gst_debug_category_get_name (category);
  log_info.file = (gchar*) file;
  log_info.function = (gchar*) function;
  log_info.line = line;

  if (GST_IS_OBJECT (object)) {
    log_info.object = GST_OBJECT_NAME (object);
  } else {
    log_info.object = (gchar*) G_OBJECT_TYPE_NAME (object);
  }

  log_info.message = (gchar*) gst_debug_message_get (message);

  gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_LOG_INFO;
  gst_data.log_info = &log_info;

  gst_debugserver_hooks_send_data (&log->hooks, tcp_server, &gst_data);
}

static gint
sort_by_category_name (gconstpointer a, gconstpointer b)
{
  return strcmp (gst_debug_category_get_name ((GstDebugCategory *) a),
      gst_debug_category_get_name ((GstDebugCategory *) b));
}

void gst_debugserver_log_send_debug_categories (GstDebugserverTcp *tcp_server, TcpClient *client)
{
  GstDebugger__GStreamerData gst_data = GST_DEBUGGER__GSTREAMER_DATA__INIT;
  GstDebugger__DebugCategories debug_categories = GST_DEBUGGER__DEBUG_CATEGORIES__INIT;
  gint categories_count, i = 0;

  gst_data.info_type_case = GST_DEBUGGER__GSTREAMER_DATA__INFO_TYPE_DEBUG_CATEGORIES;

  GSList *tmp, *all_categories = gst_debug_get_all_categories ();

  tmp = all_categories = g_slist_sort (all_categories, sort_by_category_name);
  categories_count = g_slist_length (all_categories);

  debug_categories.n_category = categories_count;
  debug_categories.category = (char **) g_malloc (sizeof (char*) * categories_count);

  while (tmp) {
    GstDebugCategory *cat = (GstDebugCategory *) tmp->data;
    debug_categories.category[i++] = (char *) gst_debug_category_get_name (cat);
    tmp = g_slist_next (tmp);
  }
  g_slist_free (all_categories);

  gst_data.debug_categories = &debug_categories;

  gst_debugserver_tcp_send_packet (tcp_server, client, &gst_data);

  g_free (debug_categories.category);
}

void gst_debugserver_log_set_threshold (const gchar * threshold)
{
  gst_debug_set_threshold_from_string (threshold, TRUE);
}

void gst_debugserver_log_remove_client (GstDebugserverLog * log,
  TcpClient * client)
{
  gst_debugserver_hooks_remove_client (&log->hooks, client);
}
