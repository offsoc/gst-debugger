/*
 * buffer_module.cpp
 *
 *  Created on: Oct 3, 2015
 *      Author: loganek
 */


#include "buffer_module.h"
#include "ui_utils.h"

#include "dialogs/buffer_data_dialog.h"

#include "controller/controller.h"

BufferModule::BufferModule()
: BaseMainModule(GstDebugger::GStreamerData::kBufferInfo, "buffers")
{
	data_dialog = load_dialog<BufferDataDialog>("bufferDataDialog");
}

void BufferModule::load_details(gpointer data)
{
	auto buffer_info = (GstDebugger::BufferInfo*)data;

	append_details_row("Pts", std::to_string(buffer_info->pts()));
	append_details_row("Dts", std::to_string(buffer_info->dts()));
	append_details_row("Duration", std::to_string(buffer_info->duration()));
	append_details_row("Offset", std::to_string(buffer_info->offset()));
	append_details_row("Offset End", std::to_string(buffer_info->offset_end()));
	append_details_row("Data Size", std::to_string(buffer_info->size()));
	append_details_row("Object path", buffer_info->pad());

	if (buffer_info->has_data())
	{
		buffer = buffer_info->data();
		append_details_row("Data", buffer_data_to_string(StringDataFormat::HEX, buffer, 1024, 16));
	}
}


void BufferModule::details_activated(const Gtk::TreeModel::Path &path)
{
	Gtk::TreeModel::iterator iter = details_model->get_iter(path);
	if (!iter)
	{
		return;
	}

	Gtk::TreeModel::Row row = *iter;
	if (row[detail_columns.name] == "Data")
	{
		data_dialog->set_data(buffer);
		data_dialog->show();
	}
}

void BufferModule::data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data)
{
	row[columns.header] = "Buffer of size " + std::to_string(data->buffer_info().size());
	row[columns.data] = new GstDebugger::BufferInfo(data->buffer_info());
}

BufferControlModule::BufferControlModule()
: ControlModule()
{
	data_check_button = Gtk::manage(new Gtk::CheckButton("Send data"));
	main_box->pack_start(*data_check_button, false, true);
	main_box->reorder_child(*data_check_button, 1);

	pad_path_label = Gtk::manage(new Gtk::Label());
	create_description_box("Pad: ", pad_path_label, 0);

	create_dispatcher("selected-object", [this] {
		auto pad_path = controller->get_selected_pad_path();
		if (pad_path.empty())
			pad_path = "none (any path)";
		pad_path_label->set_text(pad_path);
	}, nullptr);

	hooks_tree_view->append_column("Pad", hooks_model_columns.str1);
	hooks_tree_view->append_column("With data", hooks_model_columns.str2);
}

void BufferControlModule::add_hook()
{
	controller->send_buffer_request_command(true, controller->get_selected_pad_path(),
			data_check_button->get_active());
}

void BufferControlModule::remove_hook(const Gtk::TreeModel::Row& row)
{
	Glib::ustring pad_path = row[hooks_model_columns.str1];
	controller->send_buffer_request_command(false, pad_path,
			data_check_button->get_active());
}

void BufferControlModule::confirmation_received(GstDebugger::Command* cmd)
{
	if (!cmd->has_hook_request() || !cmd->hook_request().has_pad_hook() || !cmd->hook_request().pad_hook().has_buffer())
		return;

	auto confirmation = cmd->hook_request().pad_hook();
	if (cmd->hook_request().action() == GstDebugger::ADD)
	{
		Gtk::TreeModel::Row row = *(hooks_model->append());
		row[hooks_model_columns.str1] = confirmation.pad();
		row[hooks_model_columns.str2] = confirmation.buffer().send_data() ? "yes" : "no";
	}
	else
	{
		remove_confirmation_hook(confirmation);
	}
}

void BufferControlModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	ControlModule::set_controller(controller);

	controller->on_selected_object_changed.connect([this] {
		gui_emit("selected-object");
	});
}
