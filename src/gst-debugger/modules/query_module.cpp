/*
 * query_module.cpp
 *
 *  Created on: Sep 29, 2015
 *      Author: loganek
 */

#include "query_module.h"

#include "controller/controller.h"

#include <glibmm/i18n.h>

QueryModule::QueryModule()
: BaseMainModule(GstDebugger::GStreamerData::kQueryInfo, _("Queries"))
{
}

void QueryModule::load_details(gpointer data)
{
	auto query_info = (GstDebugger::QueryInfo*)data;

	append_details_row(_("query type"), Gst::Enums::get_name((Gst::QueryType)query_info->type()));
	append_details_row(_("sent from pad"), query_info->pad());

	auto structure = Glib::wrap(gst_structure_from_string(query_info->structure_data().c_str(), NULL), false);
	append_details_from_structure(structure);
}

void QueryModule::data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data)
{
	row[columns.header] = _("Query of type: ") + Gst::Enums::get_name((Gst::QueryType)data->query_info().type());
	row[columns.data] = new GstDebugger::QueryInfo(data->query_info());
}

QueryControlModule::QueryControlModule()
: ControlModule(),
  QEControlModule("GstQueryType")
{
}

void QueryControlModule::add_hook()
{
	auto it = types_combobox->get_active();
	if (it)
	{
		Gtk::TreeModel::Row row = *it;
		controller->send_query_request_command(true, controller->get_selected_pad_path(), row[types_model_columns.type_id]);
	}
}

void QueryControlModule::remove_hook(const Gtk::TreeModel::Row& row)
{
	Glib::ustring pad = row[hooks_model_columns.str2];
	controller->send_query_request_command(false, pad, row[hooks_model_columns.int1]);
}

void QueryControlModule::confirmation_received(GstDebugger::Command* cmd)
{
	if (!cmd->has_hook_request() || !cmd->hook_request().has_pad_hook() || !cmd->hook_request().pad_hook().has_query())
		return;

	auto confirmation = cmd->hook_request().pad_hook();
	if (cmd->hook_request().action() == GstDebugger::ADD)
	{
		Gtk::TreeModel::Row row = *(hooks_model->append());
		row[hooks_model_columns.str1] = Gst::Enums::get_name(static_cast<Gst::QueryType>(confirmation.query().type()));
		row[hooks_model_columns.str2] = confirmation.pad();
		row[hooks_model_columns.int1] = confirmation.query().type();
	}
	else
	{
		remove_confirmation_hook(confirmation);
	}
}
