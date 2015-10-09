/*
 * event_module.cpp
 *
 *  Created on: Sep 29, 2015
 *      Author: loganek
 */

#include "event_module.h"

#include "controller/controller.h"

#include <glibmm/i18n.h>

EventModule::EventModule()
: BaseMainModule(GstDebugger::GStreamerData::kEventInfo, _("Events"))
{
}

void EventModule::load_details(gpointer data)
{
	auto evt_info = (GstDebugger::EventInfo*)data;

	append_details_row(_("event type"), Gst::Enums::get_name((Gst::EventType)evt_info->type()));
	{
		gchar buffer[20];
		snprintf(buffer, 20, "%" GST_TIME_FORMAT, GST_TIME_ARGS(evt_info->timestamp()));
		append_details_row(_("event timestamp"), buffer);
	}
	append_details_row(_("event sequence number"), std::to_string(evt_info->seqnum()));
	append_details_row(_("sent from pad"), evt_info->pad());

	auto structure = Glib::wrap(gst_structure_from_string(evt_info->structure_data().c_str(), NULL), false);
	append_details_from_structure(structure);
}

void EventModule::data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data)
{
	row[columns.header] = _("Event of type: ") + Gst::Enums::get_name((Gst::EventType)data->event_info().type());
	row[columns.data] = new GstDebugger::EventInfo(data->event_info());
}

EventControlModule::EventControlModule()
: ControlModule(),
  QEControlModule("GstEventType")
{
}

void EventControlModule::add_hook()
{
	auto it = types_combobox->get_active();
	if (it)
	{
		Gtk::TreeModel::Row row = *it;
		controller->send_event_request_command(true, controller->get_selected_pad_path(), row[types_model_columns.type_id]);
	}
}

void EventControlModule::remove_hook(const Gtk::TreeModel::Row& row)
{
	Glib::ustring pad = row[hooks_model_columns.str2];
	controller->send_event_request_command(false, pad, row[hooks_model_columns.int1]);
}

void EventControlModule::confirmation_received(GstDebugger::Command* cmd)
{
	if (!cmd->has_hook_request() || !cmd->hook_request().has_pad_hook() || !cmd->hook_request().pad_hook().has_event())
		return;

	auto confirmation = cmd->hook_request().pad_hook();
	if (cmd->hook_request().action() == GstDebugger::ADD)
	{
		Gtk::TreeModel::Row row = *(hooks_model->append());
		row[hooks_model_columns.str1] = Gst::Enums::get_name(static_cast<Gst::EventType>(confirmation.event().type()));
		row[hooks_model_columns.str2] = confirmation.pad();
		row[hooks_model_columns.int1] = confirmation.event().type();
	}
	else
	{
		remove_confirmation_hook(confirmation);
	}
}
