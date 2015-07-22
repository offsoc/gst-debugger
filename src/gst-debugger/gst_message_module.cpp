/*
 * gst_message_module.cpp
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#include "gst_message_module.h"
#include "gvalue-converter/gvalue_enum.h"
#include "sigc++lambdahack.h"
#include "protocol/deserializer.h"

#include <gstreamermm.h>

GstMessageModule::GstMessageModule(const Glib::RefPtr<Gtk::Builder>& builder,
		const std::shared_ptr<TcpClient>& client)
: GstQEModule(true, false, GstreamerInfo_InfoType_MESSAGE,
		"BusMessage", gst_message_type_get_type(), builder, client)
{

}
void GstMessageModule::append_qe_entry()
{
	auto gstmsg = info.qebm();

	GstMessage *msg= gst_message_deserialize(gstmsg.payload().c_str(), gstmsg.payload().length());

	if (msg == NULL)
	{
		// todo log about it
		return;
	}

	Gtk::TreeModel::Row row = *(qe_list_model->append());
	row[qe_list_model_columns.type] = std::string("Message ") + Gst::Enums::get_name(static_cast<Gst::MessageType>(msg->type));
	row[qe_list_model_columns.qe] = GST_MINI_OBJECT(msg);
}

void GstMessageModule::display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe)
{
	GstQEModule::display_qe_details(qe);

	Glib::RefPtr<Gst::Message> message = message.cast_static(qe);

	append_details_row("message type", Gst::Enums::get_name(message->get_message_type()));
	{
		gchar buffer[20];
		snprintf(buffer, 20, "%" GST_TIME_FORMAT, GST_TIME_ARGS(message->get_timestamp()));
		append_details_row("message timestamp", buffer);
	}
	append_details_row("message sequence number", std::to_string(message->get_seqnum()));
	append_details_row("object", "todo");

	auto structure = message->get_structure();
	append_details_from_structure(structure);
}

void GstMessageModule::update_hook_list()
{
	auto msg_watch = info.bus_msg_confirmation();

	Gtk::TreeModel::Row row = *(qe_hooks_model->append());
	row[qe_hooks_model_columns.qe_type] = msg_watch.message_type();
}

void GstMessageModule::send_start_stop_command(bool enable)
{
	int msg_type = -1;

	if (!any_qe_check_button->get_active())
	{
		Gtk::TreeModel::iterator iter = qe_types_combobox->get_active();
		if (!iter)
			return;

		Gtk::TreeModel::Row row = *iter;
		if (!row)
			return;

		msg_type = row[qe_types_model_columns.type_id];
	}

	MessageWatch *msg_watch = new MessageWatch();
	msg_watch->set_message_type(msg_type);
	msg_watch->set_toggle(ENABLE);
	Command cmd;
	cmd.set_command_type(Command_CommandType_MESSAGE_WATCH);
	cmd.set_allocated_message_watch(msg_watch);

	client->send_command(cmd);
}
