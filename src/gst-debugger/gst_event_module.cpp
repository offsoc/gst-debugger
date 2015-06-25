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

	row[event_list_model_columns.type] = event->type;
	row[event_list_model_columns.timestamp] = event->timestamp;
	row[event_list_model_columns.seqnum] = event->seqnum;

	gst_event_unref(event);
}
