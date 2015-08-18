/*
 * gst_buffer_module.cpp
 *
 *  Created on: Jul 5, 2015
 *      Author: mkolny
 */

#include "gst_buffer_module.h"
#include "utils/deserializer.h"
#include "controller/controller.h"
#include "ui_utils.h"

GstBufferModule::GstBufferModule(const Glib::RefPtr<Gtk::Builder>& builder)
: GstQEModule(false, true, GstreamerInfo_InfoType_BUFFER,
		"Buffer", gst_buffer_get_type(), builder)
{
	qe_details_tree_view->signal_row_activated().connect(sigc::mem_fun(*this, &GstBufferModule::qeDetialsTreeView_row_activated_cb));
	data_dialog = load_dialog<BufferDataDialog>("bufferDataDialog");

	{
		Gtk::Window *wnd;
		builder->get_widget("mainWindow", wnd);
		data_dialog->set_transient_for(*wnd);
	}
}

void GstBufferModule::append_qe_entry(GstreamerQEBM *qebm)
{
	GstBuffer *buffer = gst_buffer_deserialize(qebm->payload().c_str(), qebm->payload().length());

	if (buffer == NULL)
	{
		controller->log("cannot deserialize buffer in qe module");
		return;
	}

	Gtk::TreeModel::Row row = *(qe_list_model->append());
	row[qe_list_model_columns.type] = "Buffer with size " + std::to_string(gst_buffer_get_size(buffer)) +
			", timestamp: " + std::to_string(buffer->dts);
	row[qe_list_model_columns.qe] = GST_MINI_OBJECT(buffer);
	row[qe_list_model_columns.pad_path] = qebm->pad_path();
}

void GstBufferModule::display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe, const std::string &pad_path)
{
	GstQEModule::display_qe_details(qe, pad_path);

	buffer = buffer.cast_static(qe);

	append_details_row("pad path", pad_path);
	append_details_row("pts", std::to_string(buffer->get_pts()));
	append_details_row("dts", std::to_string(buffer->get_dts()));
	append_details_row("duration", std::to_string(buffer->get_duration()));
	append_details_row("offset", std::to_string(buffer->get_offset()));
	append_details_row("offset_end", std::to_string(buffer->get_offset_end()));
	append_details_row("size", std::to_string(buffer->get_size()));
	append_details_row("flags", std::to_string(buffer->get_flags()));

	append_details_row("data", buffer_data_to_string(StringDataFormat::HEX, buffer, 1024, 16));

}

void GstBufferModule::qeDetialsTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
	Gtk::TreeModel::iterator iter = qe_details_model->get_iter(path);
	if (!iter)
	{
		return;
	}

	Gtk::TreeModel::Row row = *iter;
	if (row[qe_details_model_columns.name] == "data")
	{
		data_dialog->set_buffer(buffer);
		data_dialog->show();
	}
}
