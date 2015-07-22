/*
 * gst_buffer_module.cpp
 *
 *  Created on: Jul 5, 2015
 *      Author: mkolny
 */

#include "gst_buffer_module.h"
#include "protocol/deserializer.h"

#include <sstream>
#include <iomanip>

GstBufferModule::GstBufferModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<TcpClient>& client)
: GstQEModule(false, true, GstreamerInfo_InfoType_BUFFER,
		"Buffer", gst_query_type_get_type(), builder, client)
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
	row[qe_list_model_columns.type] = "Buffer with size " + std::to_string(gst_buffer_get_size(buffer)) +
			", timestamp: " + std::to_string(buffer->dts);
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
	append_details_row("flags", std::to_string(buffer->get_flags()));

	Glib::RefPtr<Gst::MapInfo> map_info(new Gst::MapInfo());
	buffer->map(map_info, Gst::MAP_READ);

	std::ostringstream ss;
	ss << std::hex << std::setfill('0');

	for (std::size_t i = 0; i < buffer->get_size(); i++)
	{
		if (i != 0 && i % 16 == 0)
			ss << std::endl;
		ss << std::setw(2) << static_cast<int>(map_info->get_data()[i]) << " ";
	}

	append_details_row("data", ss.str());
	buffer->unmap(map_info);
}
