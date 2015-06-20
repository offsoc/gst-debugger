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

int main (int argc, char **argv)
{
	gst_init(&argc, &argv);
	GstQuery * q = gst_query_new_duration (GST_FORMAT_PERCENT);
	gst_query_set_duration(q, GST_FORMAT_PERCENT, 59);
	gchar st_b[1025];
	gchar * tmp = st_b;
	int size = gst_query_serialize(q, tmp, 1024);
	bool need_free = false;

	if (size > 1024)
	{
		tmp = (gchar*)malloc(size);
		need_free = true;
		gst_query_serialize(q, tmp, 1024);
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

	return 0;
}
