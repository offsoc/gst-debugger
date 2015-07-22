/*
 * gst_log_module.cpp
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#include "gst_log_module.h"

#include "controller/command_factory.h"

#include <fstream>

GstLogModule::GstLogModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<TcpClient>& client)
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
}

void GstLogModule::setThresholdButton_clicked_cb()
{
	client->send_command(CommandFactory::make_set_threshold_command(log_threshold_entry->get_text(),
			overwrite_current_threshold_check_button->get_active()));
}

void GstLogModule::watchLogCheckButton_toggled_cb()
{
	client->send_command(CommandFactory::make_set_log_watch_command(watch_log_check_button->get_active(), 10)); // todo log level
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
	client->send_command(CommandFactory::make_request_debug_categories_command());
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
