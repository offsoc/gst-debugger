/*
 * serializer-test.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include <iostream>
extern "C" {
#include "protocol/serializer.h"
#include "protocol/deserializer.h"
#include "protocol/protocol_utils.h"
}

void query_serialization()
{
	GstQuery * q = gst_query_new_duration (GST_FORMAT_PERCENT);
	gst_query_set_duration(q, GST_FORMAT_PERCENT, 59);
	gchar st_b[1024];
	gchar * tmp = st_b;
	int size = gst_query_serialize(q, tmp, 1024);
	bool need_free = false;

	if (size > 1024)
	{
		tmp = (gchar*)malloc(size);
		need_free = true;
		gst_query_serialize(q, tmp, size);
	}
	gst_query_unref(q);

	GstQuery * nq = gst_query_deserialize(tmp, size);
	GstFormat fmt;
	gint64 duration;
	gst_query_parse_duration(nq, &fmt, &duration);
	std::cout << gst_query_type_get_name(nq->type) << " " << fmt << " " << duration<< std::endl;
	gst_query_unref(nq);
	if (need_free)
		free(tmp);
}

void event_serialization()
{
	GstEvent *event = gst_event_new_buffer_size(GST_FORMAT_BYTES, 500, 102314, TRUE);
	gchar st_b[1024];
	gchar * tmp = st_b;
	int size = gst_event_serialize(event, tmp, 1024);
	bool need_free = false;

	if (size > 1024)
	{
		tmp = (gchar*)malloc(size);
		need_free = true;
		gst_event_serialize(event, tmp, size);
	}
	gst_event_unref(event);

	GstEvent * ne = gst_event_deserialize(tmp, size);
	GstFormat fmt;
	gint64 minsize, maxsize;
	gboolean async;
	gst_event_parse_buffer_size(ne, &fmt, &minsize, &maxsize, &async);
	std::cout << gst_event_type_get_name(ne->type) << " " << fmt << " " << minsize << " " << maxsize << " " << async << std::endl;
	gst_event_unref(ne);
	if (need_free)
		free(tmp);
}

int main (int argc, char **argv)
{
	gst_init(&argc, &argv);

	query_serialization();
	event_serialization();

	return 0;
}
