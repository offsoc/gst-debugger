/*
 * gst_buffer_module.cpp
 *
 *  Created on: Jul 5, 2015
 *      Author: mkolny
 */

#include "gst_buffer_module.h"
#include "protocol/deserializer.h"

GstBufferModule::GstBufferModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client)
: GstQEModule(true, true, GstreamerInfo_InfoType_QUERY,
		"Query", gst_query_type_get_type(), builder, client)
{
}

void GstBufferModule::append_qe_entry()
{
	auto gstbuffer = info.qebm();

	GstBuffer *buffer = gst_buffer_deserialize(gstbuffer.payload().c_str(), gstbuffer.payload().length());

	if (buffer == NULL)
	{
		// todo log about it
		return;
	}

	Gtk::TreeModel::Row row = *(qe_list_model->append());
	row[qe_list_model_columns.type] = buffer->dts; // todo
	row[qe_list_model_columns.qe] = GST_MINI_OBJECT(buffer);
}

void GstBufferModule::display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe)
{
	GstQEModule::display_qe_details(qe);

	Glib::RefPtr<Gst::Buffer> buffer = buffer.cast_static(qe);

	append_details_row("pts", std::to_string(buffer->get_pts()));
	append_details_row("dts", std::to_string(buffer->get_dts()));
	append_details_row("duration", std::to_string(buffer->get_duration()));
	append_details_row("offset", std::to_string(buffer->get_offset()));
	append_details_row("offset_end", std::to_string(buffer->get_offset_end()));
	append_details_row("size", std::to_string(buffer->get_size()));
	append_details_row("", std::to_string(buffer->get_size()));
}
