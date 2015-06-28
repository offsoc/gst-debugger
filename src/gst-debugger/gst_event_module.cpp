/*
 * gst_event_module.cpp
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#include "gst_event_module.h"
#include "sigc++lambdahack.h"
#include "gvalue-converter/gvalue_base.h"

extern "C" {
#include "protocol/deserializer.h"
}

GstEventModule::GstEventModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client)
: client(client)
{
	builder->get_widget("eventListTreeView", event_list_tree_view);
	event_list_tree_view->signal_row_activated().connect(sigc::mem_fun(*this, &GstEventModule::eventListTreeView_row_activated_cb));
	event_list_model = Gtk::ListStore::create(event_list_model_columns);
	event_list_tree_view->set_model(event_list_model);
	event_list_tree_view->append_column("Type", event_list_model_columns.type);
	event_list_tree_view->append_column("Timestamp", event_list_model_columns.timestamp);
	event_list_tree_view->append_column("Sequence number", event_list_model_columns.seqnum);

	builder->get_widget("eventDetailsTreeView", event_details_tree_view);
	event_details_model = Gtk::TreeStore::create(event_details_model_columns);
	event_details_tree_view->set_model(event_details_model);
	event_details_tree_view->append_column("Name", event_details_model_columns.name);
	event_details_tree_view->append_column("Value", event_details_model_columns.value);

	builder->get_widget("startWatchingEventsButton", start_watching_events_button);
	start_watching_events_button->signal_clicked().connect(sigc::mem_fun(*this, &GstEventModule::startWatchingEventsButton_click_cb));
}

void GstEventModule::process_frame()
{
	switch (info.info_type())
	{
	case GstreamerInfo_InfoType_EVENT:
		append_event_entry();
		break;
	default:
		break;
	}
}

void GstEventModule::append_event_entry()
{
	auto gstevt = info.event();

	GstEvent *event = gst_event_deserialize(gstevt.payload().c_str(), gstevt.payload().length());

	if (event == NULL)
	{
		// todo log about it
		return;
	}

	Gtk::TreeModel::Row row = *(event_list_model->append());
	row[event_list_model_columns.type] = event->type;
	row[event_list_model_columns.timestamp] = event->timestamp; // todo GST_TIME_FORMAT / GST_TIME_ARGS ?
	row[event_list_model_columns.seqnum] = event->seqnum;
	row[event_list_model_columns.event] = event;
}

void GstEventModule::startWatchingEventsButton_click_cb()
{
	Command cmd;
	PadWatch *pad_watch = new PadWatch();
	pad_watch->set_toggle(ENABLE);
	pad_watch->set_watch_type(PadWatch_WatchType_EVENT);
	pad_watch->set_pad_path("todo"); // todo
	cmd.set_command_type(Command_CommandType_PAD_WATCH);
	cmd.set_allocated_pad_watch(pad_watch);
	client->send_command(cmd);
}

void GstEventModule::eventListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
	Gtk::TreeModel::iterator iter = event_list_model->get_iter(path);
	if (!iter)
	{
		return;
	}

	Gtk::TreeModel::Row row = *iter;
	Glib::RefPtr<Gst::Event> event = Glib::wrap(row[event_list_model_columns.event], true);
	display_event_details(event);
}

void GstEventModule::display_event_details(const Glib::RefPtr<Gst::Event>& event)
{
	event_details_model->clear();

	auto append_row = [this](const std::string &name, const std::string &value) {
		Gtk::TreeModel::Row row = *(event_details_model->append());
		row[event_details_model_columns.name] = name;
		row[event_details_model_columns.value] = value;
	};

	append_row("event type", Gst::Enums::get_name(event->get_event_type()));
	{
		gchar buffer[20];
		snprintf(buffer, 20, "%" GST_TIME_FORMAT, GST_TIME_ARGS(event->get_timestamp()));
		append_row("event timestamp", buffer);
	}
	append_row("event sequence number", std::to_string(event->get_seqnum()));

	auto structure = event->get_structure();

	structure.foreach([append_row, structure](const Glib::ustring &name, const Glib::ValueBase &value) -> bool {
		auto gvalue = GValueBase::build_gvalue(const_cast<GValue*>(value.gobj()));
		if (gvalue == nullptr)
			append_row(name, std::string("<unsupported type ") + g_type_name(G_VALUE_TYPE(value.gobj())) + ">");
		else
		{
			append_row(name, gvalue->to_string());
			delete gvalue;
		}
		return true;
	});
}
