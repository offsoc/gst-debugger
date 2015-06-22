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
#include "protocol/gstdebugger.pb-c.h"

#include <string.h>

GstDebugserverLog * gst_debugserver_log_new (void)
{
  GstDebugserverLog *log = (GstDebugserverLog*)g_malloc (sizeof(GstDebugserverLog));
  log->clients = NULL;

  return log;
}

void gst_debugserver_log_free (GstDebugserverLog * log)
{
  g_slist_free (log->clients);
  g_free (log);
}

void gst_debugserver_log_set_watch (GstDebugserverLog * log, gboolean enable,
  gpointer client_info)
{
  if (enable == TRUE) {
    if (g_slist_find (log->clients, client_info) == NULL) {
      log->clients = g_slist_append (log->clients, client_info);
    }
  } else {
    log->clients = g_slist_remove (log->clients, client_info);
  }
}

GSList* gst_debugserver_log_get_clients (GstDebugserverLog * log)
{
  return log->clients;
}

gint gst_debugserver_log_prepare_buffer (GstDebugCategory * category,
  GstDebugLevel level, const gchar * file, const gchar * function, gint line,
  GObject * object, GstDebugMessage * message, gchar * buffer, gint max_size)
{
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  GstreamerLog log = GSTREAMER_LOG__INIT;
  gint size;

  log.level = (gint)level;
  log.category_name = g_strdup (gst_debug_category_get_name (category));
  log.file = g_strdup (file);
  log.function = g_strdup (function);
  log.line = line;
  log.object_path = g_strdup ("todo");
  log.message = g_strdup (gst_debug_message_get (message));
  info.info_type = GSTREAMER_INFO__INFO_TYPE__LOG;
  info.log = &log;
  size = gstreamer_info__get_packed_size (&info);
  assert(size <= max_size);
  gstreamer_info__pack (&info, (guint8*)buffer);

  return size;
}

static gint
sort_by_category_name (gconstpointer a, gconstpointer b)
{
  return strcmp (gst_debug_category_get_name ((GstDebugCategory *) a),
      gst_debug_category_get_name ((GstDebugCategory *) b));
}

gint gst_debugserver_log_prepare_categories_buffer (gchar * buffer, gint max_size)
{
  GSList *tmp, *all_categories = gst_debug_get_all_categories ();
  GString *categories = g_string_new (NULL);
  GstreamerInfo info = GSTREAMER_INFO__INIT;
  DebugCategoryList category_list = DEBUG_CATEGORY_LIST__INIT;
  gint size;

  tmp = all_categories = g_slist_sort (all_categories, sort_by_category_name);

  while (tmp) {
    GstDebugCategory *cat = (GstDebugCategory *) tmp->data;
    g_string_append (categories, gst_debug_category_get_name (cat));
    g_string_append_c (categories, ';');
    tmp = g_slist_next (tmp);
  }

  category_list.list = g_strdup (categories->str);
  info.info_type = GSTREAMER_INFO__INFO_TYPE__DEBUG_CATEGORIES;
  info.debug_categories = &category_list;
  size = gstreamer_info__get_packed_size (&info);
  assert(size <= max_size);
  gstreamer_info__pack (&info, (guint8*)buffer);

  g_string_free (categories, TRUE);
  g_slist_free (all_categories);

  return size;

}
