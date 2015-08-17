/*
 * gst_message_module.cpp
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#include "gst_message_module.h"
#include "gvalue-converter/gvalue_enum.h"
#include "sigc++lambdahack.h"
#include "common/deserializer.h"
#include "controller/command_factory.h"
#include "controller/controller.h"
#include <gstreamermm.h>

GstMessageModule::GstMessageModule(const Glib::RefPtr<Gtk::Builder>& builder)
: GstQEModule(true, false, GstreamerInfo_InfoType_MESSAGE,
		"BusMessage", gst_message_type_get_type(), builder)
{
}

void GstMessageModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	GstQEModule::set_controller(controller);
	controller->on_message_confirmation_received.connect(sigc::mem_fun(*this, &GstMessageModule::message_confirmation_received));
}

void GstMessageModule::append_qe_entry(GstreamerQEBM *qebm)
{
	GstMessage *msg = gst_message_deserialize(qebm->payload().c_str(), qebm->payload().length());

	if (msg == NULL)
	{
		controller->log("cannot deserialize message in qe module");
		return;
	}

	Gtk::TreeModel::Row row = *(qe_list_model->append());
	row[qe_list_model_columns.type] = std::string("Message ") + Gst::Enums::get_name(static_cast<Gst::MessageType>(msg->type));
	row[qe_list_model_columns.qe] = GST_MINI_OBJECT(msg);
}

void GstMessageModule::display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe, const std::string &pad_path)
{
	GstQEModule::display_qe_details(qe, pad_path);

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

void GstMessageModule::confirmation_received_()
{
	auto confirmation = gui_pop<MessageWatch*>("confirmation");
	if (confirmation->toggle() == ENABLE)
	{
		Gtk::TreeModel::Row row = *(qe_hooks_model->append());
		row[qe_hooks_model_columns.qe_type_name] = Gst::Enums::get_name(static_cast<Gst::MessageType>(confirmation->message_type()));
		row[qe_hooks_model_columns.qe_type] = confirmation->message_type();
	}
	else
	{
		for (auto iter = qe_hooks_model->children().begin();
				iter != qe_hooks_model->children().end(); ++iter)
		{
			if ((*iter)[qe_hooks_model_columns.qe_type] == confirmation->message_type())
			{
				qe_hooks_model->erase(iter);
				break;
			}
		}
	}
	delete confirmation;

}

void GstMessageModule::message_confirmation_received(const MessageWatch& watch)
{
	gui_push("confirmation", new MessageWatch(watch));
	gui_emit("confirmation");
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

	controller->send_message_request_command(msg_type, enable);
}
