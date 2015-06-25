/*
 * gst_log_module.cpp
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#include "gst_log_module.h"

GstLogModule::GstLogModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client)
: client(client)
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
}

void GstLogModule::setThresholdButton_clicked_cb()
{
	Command cmd;
	LogThreshold *log_threshold = new LogThreshold();
	log_threshold->set_list(log_threshold_entry->get_text());
	log_threshold->set_overwrite(overwrite_current_threshold_check_button->get_active());
	cmd.set_command_type(Command_CommandType_LOG_THRESHOLD);
	cmd.set_allocated_log_threshold(log_threshold);
	client->send_command(cmd);
}

void GstLogModule::watchLogCheckButton_toggled_cb()
{
	Command cmd;
	LogWatch *log_watch = new LogWatch();
	log_watch->set_toggle(watch_log_check_button->get_active() ? ENABLE : DISABLE);
	log_watch->set_log_level(10); // todo
	cmd.set_command_type(Command_CommandType_LOG_WATCH);
	cmd.set_allocated_log_watch(log_watch);
	client->send_command(cmd);
}

void GstLogModule::refreshDebugCategoriesButton_clicked_cb()
{
	Command cmd;
	cmd.set_command_type(Command_CommandType_DEBUG_CATEGORIES);
	client->send_command(cmd);
}

void GstLogModule::process_frame()
{
	switch (info.info_type())
	{
	case GstreamerInfo_InfoType_DEBUG_CATEGORIES:
		update_debug_categories();
		break;
	case GstreamerInfo_InfoType_LOG:
		append_log_entry();
		break;
	default:
		break;
	}
}

void GstLogModule::update_debug_categories()
{
	debug_categories_combo_box_text->remove_all();
	std::string data = info.debug_categories().list();
	std::size_t pos;
	bool cnt = false;

	while ((pos = data.find(';')) != std::string::npos)
	{
		debug_categories_combo_box_text->append(data.substr(0, pos));
		data = data.substr(pos+1);
		cnt = true;
	}

	if (cnt)
		debug_categories_combo_box_text->set_active(0);
}

void GstLogModule::append_log_entry()
{
	Gtk::TreeModel::Row row = *(model->append());
	auto gstlog = info.log();
	row[model_columns.level] = gstlog.level();
	row[model_columns.category_name] = gstlog.category_name();
	row[model_columns.file] = gstlog.file();
	row[model_columns.function] = gstlog.function();
	row[model_columns.line] = gstlog.line();
	row[model_columns.object_path] = gstlog.object_path();
	row[model_columns.message] = gstlog.message();
}
