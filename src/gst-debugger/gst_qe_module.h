/*
 * gst_qe_module.h
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_QE_MODULE_H_
#define SRC_GST_DEBUGGER_GST_QE_MODULE_H_

#include "common_model_columns.h"
#include "controller/iview.h"

#include "protocol/gstdebugger.pb.h"

#include <gtkmm.h>
#include <gstreamermm.h>

class QEHooksModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	QEHooksModelColumns() {
		add(pad_path); add(qe_type);
	}

	Gtk::TreeModelColumn<Glib::ustring> pad_path;
	Gtk::TreeModelColumn<gint> qe_type;
};

class GstQEModule : public IBaseView
{
protected:
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
	ListModelColumns qe_list_model_columns;

	Glib::RefPtr<Gtk::TreeStore> qe_details_model;
	DetailsModelColumns qe_details_model_columns;

	Glib::RefPtr<Gtk::ListStore> qe_hooks_model;
	QEHooksModelColumns qe_hooks_model_columns;

	TypesModelColumns qe_types_model_columns;
	Glib::RefPtr<Gtk::ListStore> qe_types_model;

	GstreamerInfo_InfoType info_type;

	bool type_module;

	virtual void append_qe_entry(GstreamerQEBM *qebm) = 0;

	virtual void update_hook_list(PadWatch *confirmation);
	virtual void send_start_stop_command(bool enable);
	virtual void display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe);

	void append_details_row(const std::string &name, const std::string &value);
	void append_details_from_structure(Gst::Structure& structure);

	void startWatchingQEButton_click_cb();
	void stopWatchingQEButton_click_cb();

	void qeListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);

	PadWatch_WatchType get_watch_type() const;

	void qebm_received(const GstreamerQEBM &qebm, GstreamerInfo_InfoType type);
	void qebm_received_();

	virtual void confirmation_received_();
	void pad_confirmation_received(const PadWatch& watch, PadWatch_WatchType type);

public:
	GstQEModule(bool type_module, bool pad_path_module,
			GstreamerInfo_InfoType info_type, const std::string& qe_name,
			GType qe_gtype, const Glib::RefPtr<Gtk::Builder>& builder);

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_GST_QE_MODULE_H_ */
