/*
 * gst_log_module.h
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_LOG_MODULE_H_
#define SRC_GST_DEBUGGER_GST_LOG_MODULE_H_

#include "gst_debugger_tcp_client.h"
#include "frame_receiver.h"
#include "protocol/gstdebugger.pb.h"

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


class GstLogModule : public FrameReceiver
{
	Gtk::Entry *log_threshold_entry;
	Gtk::CheckButton *overwrite_current_threshold_check_button;
	Gtk::CheckButton *watch_log_check_button;
	Gtk::ComboBoxText *debug_categories_combo_box_text;
	Gtk::Button *refresh_debug_categories_button;
	Gtk::TreeView *log_messages_tree_view;
	Gtk::Button *clear_message_logs_button;
	Gtk::Button *set_threshold_button;
	Gtk::Button *save_message_logs_button;

	void setThresholdButton_clicked_cb();
	void watchLogCheckButton_toggled_cb();
	void refreshDebugCategoriesButton_clicked_cb();
	void saveMessageLogsButton_clicked_cb();

	void update_debug_categories();
	void append_log_entry();

	GstreamerLogModelColumns model_columns;
	Glib::RefPtr<Gtk::ListStore> model;

	std::shared_ptr<GstDebuggerTcpClient> client;

	void process_frame() override;
public:
	GstLogModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client);
};

#endif /* SRC_GST_DEBUGGER_GST_LOG_MODULE_H_ */
