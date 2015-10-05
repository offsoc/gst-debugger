/*
 * message_module.cpp
 *
 *  Created on: Sep 27, 2015
 *      Author: loganek
 */

#include "message_module.h"

#include "controller/controller.h"

MessageModule::MessageModule()
: BaseMainModule(GstDebugger::GStreamerData::kMessageInfo, "Messages")
{
}

void MessageModule::load_details(gpointer data)
{
	auto msg_info = (GstDebugger::MessageInfo*)data;

	append_details_row("message type", Gst::Enums::get_name((Gst::MessageType)msg_info->type()));
	{
		gchar buffer[20];
		snprintf(buffer, 20, "%" GST_TIME_FORMAT, GST_TIME_ARGS(msg_info->timestamp()));
		append_details_row("message timestamp", buffer);
	}
	append_details_row("message sequence number", std::to_string(msg_info->seqnum()));
	append_details_row("object", "todo"); // todo

	auto structure = Glib::wrap(gst_structure_from_string(msg_info->structure_data().c_str(), NULL), false);
	append_details_from_structure(structure);
}

void MessageModule::data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data)
{
	row[columns.header] = "Message of type: " + Gst::Enums::get_name((Gst::MessageType)data->message_info().type());
	row[columns.data] = new GstDebugger::MessageInfo(data->message_info());
}


MessageControlModule::MessageControlModule()
: ControlModule()
{
	types_combobox = Gtk::manage(new Gtk::ComboBox());
	types_model = Gtk::ListStore::create(types_model_columns);
	types_combobox->set_model(types_model);
	types_combobox->pack_start(types_model_columns.type_name);

	create_description_box("Type: ", types_combobox, 0);

	hooks_tree_view->append_column("Type", hooks_model_columns.str1);
}

void MessageControlModule::add_hook()
{
	auto it = types_combobox->get_active();
	if (it)
	{
		Gtk::TreeModel::Row row = *it;
		controller->send_message_request_command(row[types_model_columns.type_id], true);
	}
}

void MessageControlModule::remove_hook(const Gtk::TreeModel::Row& row)
{
	controller->send_message_request_command(row[hooks_model_columns.int1], false);
}

void MessageControlModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	ControlModule::set_controller(controller);

	controller->on_enum_list_changed.connect([this](const Glib::ustring& name, bool add) {
		if (name != "GstMessageType")
			return;
		types_model->clear();
		if (add)
		{
			boost::optional<GstEnumType> type = this->controller->get_enum_type(name);
			if (!type)
				return;

			for (auto t : type.get().get_values())
			{
				Gtk::TreeModel::Row row = *(types_model->append());
				row[types_model_columns.type_id] = t.first;
				row[types_model_columns.type_name] = t.second.name;
			}

			if (!type.get().get_values().empty())
				types_combobox->set_active(0);
		}
	});
}

void MessageControlModule::confirmation_received(GstDebugger::Command* cmd)
{
	if (!cmd->has_hook_request() || !cmd->hook_request().has_message())
		return;

	auto confirmation = cmd->hook_request().message();
	if (cmd->hook_request().action() == GstDebugger::ADD)
	{
		Gtk::TreeModel::Row row = *(hooks_model->append());
		row[hooks_model_columns.str1] = Gst::Enums::get_name(static_cast<Gst::MessageType>(confirmation.type()));
		row[hooks_model_columns.int1] = confirmation.type();
	}
	else
	{
		remove_confirmation_hook(confirmation);
	}
}
