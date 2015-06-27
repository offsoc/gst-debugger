/*
 * gst_event_module.cpp
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#include "gst_event_module.h"

extern "C" {
#include "protocol/deserializer.h"
}

GstEventModule::GstEventModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client)
: client(client)
{
	builder->get_widget("eventListTreeView", event_list_tree_view);
	event_list_model = Gtk::ListStore::create(event_list_model_columns);
	event_list_tree_view->set_model(event_list_model);
	event_list_tree_view->append_column("Type", event_list_model_columns.type);
	event_list_tree_view->append_column("Timestamp", event_list_model_columns.timestamp);
	event_list_tree_view->append_column("Sequence number", event_list_model_columns.seqnum);

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
	Gtk::TreeModel::Row row = *(event_list_model->append());
	auto gstevt = info.event();

	GstEvent *event = gst_event_deserialize(gstevt.payload().c_str(), gstevt.payload().length());

	if (event == NULL)
	{
		// todo log about it
		return;
	}

	row[event_list_model_columns.type] = event->type;
	row[event_list_model_columns.timestamp] = event->timestamp;
	row[event_list_model_columns.seqnum] = event->seqnum;

	gst_event_unref(event);
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
