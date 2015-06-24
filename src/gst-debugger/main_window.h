/*
 * main_window.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_MAIN_WINDOW_H_
#define SRC_GST_DEBUGGER_MAIN_WINDOW_H_

#include "gst_debugger_tcp_client.h"
#include "connection_properties_dialog.h"

#include <gtkmm.h>

class GstreamerLogModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

	GstreamerLogModelColumns() {
		add(level); add(category_name); add(file);
		add(function); add(line); add(object_path); add(message);
	}

	Gtk::TreeModelColumn<gint32> level;
	Gtk::TreeModelColumn<Glib::ustring> category_name;
	Gtk::TreeModelColumn<Glib::ustring> file;
	Gtk::TreeModelColumn<Glib::ustring> function;
	Gtk::TreeModelColumn<gint32> line;
	Gtk::TreeModelColumn<Glib::ustring> object_path;
	Gtk::TreeModelColumn<Glib::ustring> message;
};

class MainWindow : public Gtk::Window
{
	void connectionPropertiesMenuItem_activate_cb();
	void connectMenuItem_activate_cb();
	void connection_status_changed(bool connected);
	void setThresholdButton_clicked_cb();
	void watchLogCheckButton_toggled_cb();
	void refreshDebugCategoriesButton_clicked_cb();

	GstDebuggerTcpClient client;

	Glib::RefPtr<Gtk::Builder> builder;
	Gtk::MenuItem *connection_properties;
	Gtk::ImageMenuItem *connect_menu_item;
	ConnectionPropertiesDialog *connection_properties_dialog;
	Gtk::Statusbar *main_statusbar;
	Gtk::Button *set_threshold_button;
	Gtk::Entry *log_threshold_entry;
	Gtk::CheckButton *overwrite_current_threshold_check_button;
	Gtk::CheckButton *watch_log_check_button;
	Gtk::ComboBoxText *debug_categories_combo_box_text;
	Gtk::Button *refresh_debug_categories_button;
	Gtk::TreeView *log_messages_tree_view;
	Gtk::Button *clear_message_logs_button;

	GstreamerLogModelColumns model_columns;
	Glib::RefPtr<Gtk::ListStore> model;

	Glib::Dispatcher dispatcher;

	GstreamerInfo info;

public:
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~MainWindow() {}
};

#endif /* SRC_GST_DEBUGGER_MAIN_WINDOW_H_ */
