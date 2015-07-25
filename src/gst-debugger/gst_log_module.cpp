/*
 * gst_log_module.cpp
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#include "gst_log_module.h"

#include "controller/command_factory.h"
#include "controller/controller.h"

#include <boost/algorithm/string/split.hpp>

#include <fstream>

static void free_log(GstreamerLog *log) { delete log; }
static void free_categories(DebugCategoryList *log) { delete log; }

GstLogModule::GstLogModule(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("logThresholdEntry", log_threshold_entry);
	builder->get_widget("overwriteCurrentThresholdCheckButton", overwrite_current_threshold_check_button);

	builder->get_widget("setThresholdButton", set_threshold_button);
	set_threshold_button->signal_clicked().connect(sigc::mem_fun(*this, &GstLogModule::setThresholdButton_clicked_cb));

	builder->get_widget("debugCategoriesComboBoxText", debug_categories_combo_box_text);

	builder->get_widget("watchLogCheckButton", watch_log_check_button);
	watch_log_check_button->signal_toggled().connect(sigc::mem_fun(*this, &GstLogModule::watchLogCheckButton_toggled_cb));

	builder->get_widget("refreshDebugCategoriesButton", refresh_debug_categories_button);
	refresh_debug_categories_button->signal_clicked().connect(sigc::mem_fun(*this, &GstLogModule::refreshDebugCategoriesButton_clicked_cb));

	builder->get_widget("saveMessageLogsButton", save_message_logs_button);
	save_message_logs_button->signal_clicked().connect(sigc::mem_fun(*this, &GstLogModule::saveMessageLogsButton_clicked_cb));

	builder->get_widget("clearMessageLogsButton", clear_message_logs_button);
	clear_message_logs_button->signal_clicked().connect([this] {
		model->clear();
	});

	builder->get_widget("logMessagesTreeView", log_messages_tree_view);
	model = Gtk::ListStore::create(model_columns);
	log_messages_tree_view->set_model(model);
	log_messages_tree_view->append_column("Level", model_columns.level);
	log_messages_tree_view->append_column("Category", model_columns.category_name);
	log_messages_tree_view->append_column("File", model_columns.file);
	log_messages_tree_view->append_column("Function", model_columns.function);
	log_messages_tree_view->append_column("Line", model_columns.line);
	log_messages_tree_view->append_column("Object", model_columns.object_path);
	log_messages_tree_view->append_column("Message", model_columns.message);

	create_dispatcher("log", sigc::mem_fun(*this, &GstLogModule::new_log_entry_), (GDestroyNotify)free_log);
	create_dispatcher("debug-categories", sigc::mem_fun(*this, &GstLogModule::new_debug_categories_), (GDestroyNotify)free_categories);
}

void GstLogModule::setThresholdButton_clicked_cb()
{
	controller->make_set_threshold_command(log_threshold_entry->get_text(),
			overwrite_current_threshold_check_button->get_active());
}

void GstLogModule::watchLogCheckButton_toggled_cb()
{
	controller->make_set_log_watch_command(watch_log_check_button->get_active(), 10); // todo log level
}

void GstLogModule::saveMessageLogsButton_clicked_cb()
{
	Gtk::FileChooserDialog dialog("Please choose a folder",
			Gtk::FILE_CHOOSER_ACTION_SAVE);

	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("OK", Gtk::RESPONSE_OK);

	int result = dialog.run();

	//Handle the response:
	switch(result)
	{
	case Gtk::RESPONSE_CANCEL:
		return;
	default:
		break;
	}

	std::ofstream file(dialog.get_filename());

	for (auto row : model->children())
	{
		file << row[model_columns.level] << "\t"
				<< row[model_columns.level] << "\t"
				<< row[model_columns.category_name] << "\t"
				<< row[model_columns.file] << "\t"
				<< row[model_columns.function] << "\t"
				<< row[model_columns.line] << "\t"
				<< row[model_columns.object_path] << "\t"
				<< row[model_columns.message] << std::endl;
	}
}

void GstLogModule::refreshDebugCategoriesButton_clicked_cb()
{
	controller->make_request_debug_categories_command();
}

void GstLogModule::process_frame()
{
}

void GstLogModule::new_debug_categories_()
{
	debug_categories_combo_box_text->remove_all();

	auto debug_categories = gui_pop<DebugCategoryList*>("debug-categories");

	std::vector<std::string> categories;
	boost::split(categories, debug_categories->list(), [](char c) { return c == ';'; });
	delete debug_categories;
	categories.erase(std::remove_if(categories.begin(), categories.end(),
			[](const std::string &s){return s.empty();}), categories.end());

	for (auto cat : categories)
	{
		debug_categories_combo_box_text->append(cat);
	}

	if (!categories.empty())
		debug_categories_combo_box_text->set_active(0);
}

void GstLogModule::new_debug_categories(const DebugCategoryList& debug_categories)
{
	gui_push ("debug-categories", new DebugCategoryList(debug_categories));
	gui_emit ("debug-categories");
}

void GstLogModule::new_log_entry(const GstreamerLog& log_info)
{
	gui_push("log", new GstreamerLog (log_info));
	gui_emit("log");
}

void GstLogModule::new_log_entry_()
{
	auto log_info = gui_pop<GstreamerLog*>("log");

	Gtk::TreeModel::Row row = *(model->append());
	row[model_columns.level] = log_info->level();
	row[model_columns.category_name] = log_info->category_name();
	row[model_columns.file] = log_info->file();
	row[model_columns.function] = log_info->function();
	row[model_columns.line] = log_info->line();
	row[model_columns.object_path] = log_info->object_path();
	row[model_columns.message] = log_info->message();

	delete log_info;
}

void GstLogModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_log_received.connect(sigc::mem_fun(*this, &GstLogModule::new_log_entry));
	controller->on_debug_categories_received.connect(sigc::mem_fun(*this, &GstLogModule::new_debug_categories));
}
