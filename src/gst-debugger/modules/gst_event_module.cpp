/*
 * gst_event_module.cpp
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#include "gst_event_module.h"
#include "utils/deserializer.h"
#include "controller/controller.h"

GstEventModule::GstEventModule(const Glib::RefPtr<Gtk::Builder>& builder)
: GstQEModule(true, true, GstreamerInfo_InfoType_EVENT,
		"Event", gst_event_type_get_type(), builder)
{
}

void GstEventModule::append_qe_entry(GstreamerQEBM *qebm)
{
	GstEvent *event = gst_event_deserialize(qebm->payload().c_str(), qebm->payload().length());

	if (event == NULL)
	{
		controller->log("cannot deserialize event in qe module");
		return;
	}

	Gtk::TreeModel::Row row = *(qe_list_model->append());
	row[qe_list_model_columns.type] = std::string("Event ") + Gst::Enums::get_name(static_cast<Gst::EventType>(event->type));
	row[qe_list_model_columns.qe] = GST_MINI_OBJECT(event);
	row[qe_list_model_columns.pad_path] = qebm->pad_path();
}

void GstEventModule::display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe, const std::string &pad_path)
{
	GstQEModule::display_qe_details(qe, pad_path);

	Glib::RefPtr<Gst::Event> event = event.cast_static(qe);

	append_details_row("pad path", pad_path);
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

