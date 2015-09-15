/*
 * bus_messages_module.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#include "bus_messages_module.h"
#include "filter_utils.h"

#include "controller/controller.h"

#include "common/deserializer.h"
#include "common/gstdebugger.pb.h"

static void free_bus_messages(GstreamerQEBM *qebm) { delete qebm; }

BusMessagesModule::BusMessagesModule()
{
	model = Gtk::ListStore::create(columns);
	create_dispatcher("new-message", sigc::mem_fun(*this, &BusMessagesModule::bus_message_received_), (GDestroyNotify)free_bus_messages);
}

void BusMessagesModule::configure_main_list_view(Gtk::TreeView *view)
{
	BaseMainModule::configure_main_list_view(view);
	view->append_column("Messages", columns.header);
}

void BusMessagesModule::load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path)
{
	BaseMainModule::load_details(view, path);

	Gtk::TreeModel::iterator iter = filter->get_iter(path);
	if (!iter)
	{
		return;
	}

	Gtk::TreeModel::Row row = *iter;
	auto message = Glib::wrap((GstMessage*)row[columns.message], true);

	append_details_row("message type", Gst::Enums::get_name(message->get_message_type()));
	{
		gchar buffer[20];
		snprintf(buffer, 20, "%" GST_TIME_FORMAT, GST_TIME_ARGS(message->get_timestamp()));
		append_details_row("message timestamp", buffer);
	}
	append_details_row("message sequence number", std::to_string(message->get_seqnum()));
	append_details_row("object", "todo"); // todo

	auto structure = message->get_structure();
	append_details_from_structure(structure);
}

void BusMessagesModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	BaseMainModule::set_controller(controller);

	controller->on_qebm_received.connect([this] (const GstreamerQEBM &qebm, GstreamerInfo_InfoType type) {
		if (type == GstreamerInfo_InfoType_MESSAGE)
		{
			gui_push("new-message", new GstreamerQEBM (qebm));
			gui_emit("new-message");
		}
	});
}

void BusMessagesModule::bus_message_received_()
{
	auto qebm = gui_pop<GstreamerQEBM*>("new-message");
	Gtk::TreeModel::Row row = *(model->append());
	GstMessage *message = gst_message_deserialize (qebm->payload().c_str(), qebm->payload().length());
	row[columns.header] = "Message";
	row[columns.message] = message;
	delete qebm;
}

bool BusMessagesModule::filter_function(const Gtk::TreeModel::const_iterator& it)
{
	if (!filter_expression)
		return true;

	std::shared_ptr<TokenIdentifier> ident;
	std::shared_ptr<TokenBase> value;

	read_tokens_by_type(filter_expression, ident, value);

	auto obj = it->get_value(columns.message);

	if (obj == nullptr)
		return true;

	return filter_structure(gst_message_get_structure(obj), ident->get_value().c_str(), value);
}
