/*
 * gst_message_module.cpp
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#include "gst_message_module.h"
#include "gvalue-converter/gvalue_enum.h"
#include "sigc++lambdahack.h"

extern "C" {
#include "protocol/deserializer.h"
}

#include <gstreamermm.h>

GstMessageModule::GstMessageModule(const Glib::RefPtr<Gtk::Builder>& builder,
		const std::shared_ptr<GstDebuggerTcpClient>& client)
: client(client)
{
	builder->get_widget("busMessageTypesComboBox", bus_message_types_combo_box);
	message_types_model = Gtk::ListStore::create(message_types_model_columns);
	bus_message_types_combo_box->set_model(message_types_model);
	bus_message_types_combo_box->pack_start(message_types_model_columns.type_name);

	for (auto val : GValueEnum::get_values(gst_message_type_get_type()))
	{
		Gtk::TreeModel::Row row = *(message_types_model->append());
		row[message_types_model_columns.type_id] = val.first;
		row[message_types_model_columns.type_name] = val.second;
	}
	if (message_types_model->children().size() > 0)
	{
		bus_message_types_combo_box->set_active(0);
	}

	builder->get_widget("startBusMessageButton", start_bus_message_button);
	start_bus_message_button->signal_clicked().connect(sigc::mem_fun(*this, &GstMessageModule::startBusMessageButton_clicked_cb));

	builder->get_widget("existingMessageHooksTreeView", existing_message_hooks_tree_view);
	message_hooks_model = Gtk::ListStore::create(message_hooks_model_columns);
	existing_message_hooks_tree_view->set_model(message_hooks_model);
	existing_message_hooks_tree_view->append_column("Message type", message_hooks_model_columns.type);

	builder->get_widget("MessageListTreeView", message_list_tree_view);
	message_list_tree_view->signal_row_activated().connect(sigc::mem_fun(*this, &GstMessageModule::MessageListTreeView_row_activated_cb));
	msg_list_model = Gtk::ListStore::create(msg_list_model_columns);
	message_list_tree_view->set_model(msg_list_model);
	message_list_tree_view->append_column("Type", msg_list_model_columns.type);

	builder->get_widget("detailsMessageTreeView", message_details_tree_view);
	msg_details_model = Gtk::TreeStore::create(msg_details_model_columns);
	message_details_tree_view->set_model(msg_details_model);
	message_details_tree_view->append_column("Name", msg_details_model_columns.name);
	message_details_tree_view->append_column("Value", msg_details_model_columns.value);
}

void GstMessageModule::process_frame()
{
	if (info.info_type() == GstreamerInfo_InfoType_MESSAGE)
		append_message_entry();
	else if (info.info_type() == GstreamerInfo_InfoType_MESSAGE_CONFIRMATION)
		update_hook_list();
}

void GstMessageModule::append_message_entry()
{
	GstMessage *msg = gst_message_deserialize (info.qebm().payload().c_str(),
			info.qebm().payload().length());

	Gtk::TreeModel::Row row = *(msg_list_model->append());
	row[msg_list_model_columns.type] = msg->type;
	row[msg_list_model_columns.qe] = GST_MINI_OBJECT(msg);
}

void GstMessageModule::update_hook_list()
{
	auto msg_watch = info.bus_msg_confirmation();

	Gtk::TreeModel::Row row = *(message_hooks_model->append());
	row[message_hooks_model_columns.type] = msg_watch.message_type();
}

void GstMessageModule::startBusMessageButton_clicked_cb()
{

	Gtk::TreeModel::iterator iter = bus_message_types_combo_box->get_active();
	if (!iter)
		return;

	Gtk::TreeModel::Row row = *iter;
	if (!row)
		return;

	int msg_type = row[message_types_model_columns.type_id];

	MessageWatch *msg_watch = new MessageWatch();
	msg_watch->set_message_type(msg_type);
	msg_watch->set_toggle(ENABLE);
	Command cmd;
	cmd.set_command_type(Command_CommandType_MESSAGE_WATCH);
	cmd.set_allocated_message_watch(msg_watch);

	client->send_command(cmd);
}

void GstMessageModule::append_details_row(const std::string &name, const std::string &value)
{
	Gtk::TreeModel::Row row = *(msg_details_model->append());
	row[msg_details_model_columns.name] = name;
	row[msg_details_model_columns.value] = value;
}

void GstMessageModule::append_details_from_structure(Gst::Structure& structure)
{
	if (!structure.gobj())
		return;

	structure.foreach([structure, this](const Glib::ustring &name, const Glib::ValueBase &value) -> bool {
		auto gvalue = GValueBase::build_gvalue(const_cast<GValue*>(value.gobj()));
		if (gvalue == nullptr)
			append_details_row(name, std::string("<unsupported type ") + g_type_name(G_VALUE_TYPE(value.gobj())) + ">");
		else
		{
			append_details_row(name, gvalue->to_string());
			delete gvalue;
		}
		return true;
	});
}

void GstMessageModule::MessageListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
	Gtk::TreeModel::iterator iter = msg_list_model->get_iter(path);
	if (!iter)
	{
		return;
	}

	Gtk::TreeModel::Row row = *iter;
	GstMiniObject *miniobj = row[msg_list_model_columns.qe];
	Glib::RefPtr<Gst::Message> message = Glib::wrap(GST_MESSAGE(miniobj), true);

	msg_details_model->clear();

	auto append_details_row = [this] (const std::string &name, const std::string &value) {
		Gtk::TreeModel::Row row = *(msg_details_model->append());
		row[msg_details_model_columns.name] = name;
		row[msg_details_model_columns.value] = value;
	};

	append_details_row("event type", Gst::Enums::get_name(message->get_message_type()));
	{
		gchar buffer[20];
		snprintf(buffer, 20, "%" GST_TIME_FORMAT, GST_TIME_ARGS(message->get_timestamp()));
		append_details_row("event timestamp", buffer);
	}
	append_details_row("event sequence number", std::to_string(message->get_seqnum()));

	auto structure = message->get_structure();
	append_details_from_structure(structure);
}
