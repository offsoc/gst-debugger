/*
 * gst_qe_module.h
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_QE_MODULE_H_
#define SRC_GST_DEBUGGER_GST_QE_MODULE_H_

#include "gst_debugger_tcp_client.h"
#include "frame_receiver.h"

#include <gtkmm.h>
#include <gstreamermm.h>

class QEListModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	QEListModelColumns() {
		add(type); add(qe);
	}

	Gtk::TreeModelColumn<gint32> type;
	Gtk::TreeModelColumn<GstMiniObject*> qe;
};

class QEDetailsModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	QEDetailsModelColumns() {
		add(name); add(value);
	}

	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<Glib::ustring> value;
};

class QEHooksModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	QEHooksModelColumns() {
		add(pad_path); add(qe_type);
	}

	Gtk::TreeModelColumn<Glib::ustring> pad_path;
	Gtk::TreeModelColumn<gint> qe_type;
};

class QETypesModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	QETypesModelColumns()
	{add(type_name); add(type_id);}

	Gtk::TreeModelColumn<int> type_id;
	Gtk::TreeModelColumn<Glib::ustring> type_name;
};

class GstQEModule : public FrameReceiver
{
protected:
	std::shared_ptr<GstDebuggerTcpClient> client;

	Gtk::TreeView *qe_list_tree_view;
	Gtk::TreeView *qe_details_tree_view;
	Gtk::Button *start_watching_qe_button;
	Gtk::Button *stop_watching_qe_button;
	Gtk::ComboBox *qe_types_combobox;
	Gtk::Entry *qe_pad_path_entry;
	Gtk::CheckButton *any_path_check_button;
	Gtk::CheckButton *any_qe_check_button;
	Gtk::TreeView *existing_hooks_tree_view;

	Glib::RefPtr<Gtk::ListStore> qe_list_model;
	QEListModelColumns qe_list_model_columns;

	Glib::RefPtr<Gtk::TreeStore> qe_details_model;
	QEDetailsModelColumns qe_details_model_columns;

	Glib::RefPtr<Gtk::ListStore> qe_hooks_model;
	QEHooksModelColumns qe_hooks_model_columns;

	QETypesModelColumns qe_types_model_columns;
	Glib::RefPtr<Gtk::ListStore> qe_types_model;



	void update_hook_list();
	void send_start_stop_command(bool enable, PadWatch_WatchType watch_type);
	virtual void display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe);

	void append_details_row(const std::string &name, const std::string &value);
	void append_details_from_structure(Gst::Structure& structure);

	virtual void startWatchingQEButton_click_cb() = 0;
	virtual void stopWatchingQEButton_click_cb() = 0;

	void qeListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);

public:
	GstQEModule(const std::string& qe_name, GType qe_gtype, const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client);
};

#endif /* SRC_GST_DEBUGGER_GST_QE_MODULE_H_ */
