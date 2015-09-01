/*
 * log_module.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#include "log_module.h"

#include "controller/controller.h"

static void free_log(GstreamerLog *log) { delete log; }

LogModule::LogModule()
{
	model = Gtk::ListStore::create(columns);

	create_dispatcher("new-log", sigc::mem_fun(*this, &LogModule::log_received_), (GDestroyNotify)free_log);
}

void LogModule::configure_main_list_view(Gtk::TreeView *view)
{
	BaseMainModule::configure_main_list_view(view);
	view->append_column("Logs", columns.header);
}

void LogModule::load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path)
{
	BaseMainModule::load_details(view, path);

	Gtk::TreeModel::iterator iter = filter->get_iter(path);
	if (!iter)
	{
		return;
	}

	Gtk::TreeModel::Row row = *iter;
	auto log_info = (GstreamerLog*)row[columns.log];

	append_details_row("Level", std::to_string(log_info->level()));
	append_details_row("Category name", log_info->category_name());
	append_details_row("File", log_info->file());
	append_details_row("Function", log_info->function());
	append_details_row("Line", std::to_string(log_info->line()));
	append_details_row("Object path", log_info->object_path());
	append_details_row("Message", log_info->message());
}

void LogModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	BaseMainModule::set_controller(controller);

	controller->on_log_received.connect([this] (const GstreamerLog &log) {
		gui_push("new-log", new GstreamerLog (log));
		gui_emit("new-log");
	});
}

void LogModule::log_received_()
{
	auto log = gui_pop<GstreamerLog*>("new-log");
	Gtk::TreeModel::Row row = *(model->append());
	row[columns.header] = log->function();
	row[columns.log] = log;
}

bool LogModule::filter_function(const Gtk::TreeModel::const_iterator& it)
{
	if (filter_text.empty())
		return true;

	auto log = it->get_value(columns.log);

	if (log == nullptr)
		return true;

	int line = atoi(filter_text.c_str());

	return log->line() == line;
}

LogControlModule::LogControlModule()
{
	main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

	set_watch_button = Gtk::manage(new Gtk::CheckButton("Watch log messages"));
	set_watch_button->signal_toggled().connect([this] {
		controller->send_set_log_watch_command(set_watch_button->get_active(), 10); // todo log level
	});
	main_box->pack_start(*set_watch_button, false, true);

	main_box->pack_start(*Gtk::manage(new Gtk::Label("Debug categories")), false, true);
	main_box->pack_start(*Gtk::manage(new Gtk::ComboBox()), false, true);

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
}

Gtk::Widget* LogControlModule::get_widget()
{
	return main_box;
}
