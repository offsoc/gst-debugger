/*
 * log_module.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#include "log_module.h"

#include "controller/controller.h"

LogModule::LogModule()
: BaseMainModule(GstDebugger::GStreamerData::kLogInfo, "logs")
{
}

void LogModule::load_details(gpointer data)
{
	auto log_info = (GstDebugger::LogInfo*)data;

	append_details_row("Level", std::to_string(log_info->level()));
	append_details_row("Category name", log_info->category());
	append_details_row("File", log_info->file());
	append_details_row("Function", log_info->function());
	append_details_row("Line", std::to_string(log_info->line()));
	append_details_row("Object path", log_info->object());
	append_details_row("Message", log_info->message());
}

void LogModule::data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data)
{
	row[columns.header] = data->log_info().function();
	row[columns.data] = new GstDebugger::LogInfo(data->log_info());
}

LogControlModule::LogControlModule()
: ControlModule()
{
	auto lbl = Gtk::manage(new Gtk::Label("Debug categories"));
	main_box->pack_start(*lbl, false, true);
	main_box->reorder_child(*lbl, 0);

	debug_categories_combobox = Gtk::manage(new Gtk::ComboBoxText());
	main_box->pack_start(*debug_categories_combobox, false, true);
	main_box->reorder_child(*debug_categories_combobox, 1);

	log_level_entry = Gtk::manage(new Gtk::Entry());
	main_box->pack_start(*log_level_entry, false, true);
	main_box->reorder_child(*log_level_entry, 0);

	main_box->pack_start(*Gtk::manage(new Gtk::Label("Log threshold:")), false, true);

	threshold_entry = Gtk::manage(new Gtk::Entry());
	main_box->pack_start(*threshold_entry, false, true);

	overwrite_threshold_check_button = Gtk::manage(new Gtk::CheckButton("Overwrite current threshold"));
	main_box->pack_start(*overwrite_threshold_check_button, false, true);

	set_threshold_button = Gtk::manage(new Gtk::Button("Set threshold"));
	set_threshold_button->signal_clicked().connect([this] {
		controller->send_set_threshold_command(threshold_entry->get_text(),
			overwrite_threshold_check_button->get_active());
	});
	main_box->pack_start(*set_threshold_button, false, true);

	hooks_tree_view->append_column("Level", hooks_model_columns.str1);
	hooks_tree_view->append_column("Category", hooks_model_columns.str2);
}

void LogControlModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	ControlModule::set_controller(controller);

	controller->on_debug_categories_changed.connect([this] {
		debug_categories_combobox->remove_all();
		for (auto c : this->controller->get_debug_categories())
		{
			debug_categories_combobox->append(c);
		}
		if (!this->controller->get_debug_categories().empty())
			debug_categories_combobox->set_active(0);
	});
}

void LogControlModule::add_hook()
{
	controller->send_set_log_hook_command(true, debug_categories_combobox->get_active_text(),
			atoi(log_level_entry->get_text().c_str()));
}

void LogControlModule::remove_hook(const Gtk::TreeModel::Row& row)
{
	Glib::ustring category = row[hooks_model_columns.str2];
	int level = row[hooks_model_columns.int1];
	controller->send_set_log_hook_command(false, category, level);
}

void LogControlModule::confirmation_received(GstDebugger::Command* cmd)
{
	if (!cmd->has_log())
		return;

	auto confirmation = cmd->log();
	if (confirmation.action() == GstDebugger::ADD)
	{
		Gtk::TreeModel::Row row = *(hooks_model->append());
		row[hooks_model_columns.str1] = gst_debug_level_get_name (static_cast<GstDebugLevel>(confirmation.level()));
		row[hooks_model_columns.int1] = confirmation.level();
		row[hooks_model_columns.str2] = confirmation.category();
	}
	else
	{
		remove_confirmation_hook(confirmation);
	}
}
