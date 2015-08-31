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

	Gtk::TreeModel::iterator iter = model->get_iter(path);
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


