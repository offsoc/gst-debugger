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
: GstQEModule(GstreamerInfo_InfoType_EVENT, PadWatch_WatchType_EVENT,
		"Event", gst_event_type_get_type(), builder, client)
{
}

void GstEventModule::append_qe_entry()
{
	auto gstevt = info.qebm();

	GstEvent *event = gst_event_deserialize(gstevt.payload().c_str(), gstevt.payload().length());

	if (event == NULL)
	{
		// todo log about it
		return;
	}

	Gtk::TreeModel::Row row = *(qe_list_model->append());
	row[qe_list_model_columns.type] = event->type;
/*	row[qe_list_model_columns.timestamp] = event->timestamp; // todo GST_TIME_FORMAT / GST_TIME_ARGS ?
	row[qe_list_model_columns.seqnum] = event->seqnum;*/
	row[qe_list_model_columns.qe] = GST_MINI_OBJECT(event);
}

void GstEventModule::display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe)
{
	GstQEModule::display_qe_details(qe);

	Glib::RefPtr<Gst::Event> event = event.cast_static(qe);

	append_details_row("event type", Gst::Enums::get_name(event->get_event_type()));
	{
		gchar buffer[20];
		snprintf(buffer, 20, "%" GST_TIME_FORMAT, GST_TIME_ARGS(event->get_timestamp()));
		append_details_row("event timestamp", buffer);
	}
	append_details_row("event sequence number", std::to_string(event->get_seqnum()));

	auto structure = event->get_structure();
	append_details_from_structure(structure);
}

