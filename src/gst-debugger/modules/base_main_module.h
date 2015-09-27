/*
 * base_main_module.h
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_BASE_MAIN_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_BASE_MAIN_MODULE_H_

#include "common_model_columns.h"

#include "controller/iview.h"

#include "common/gstdebugger.pb.h"

#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>

class MainModuleModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	MainModuleModelColumns() {
		add(header); add(data);
	}

	Gtk::TreeModelColumn<Glib::ustring> header;
	Gtk::TreeModelColumn<gpointer> data;
};


class BaseMainModule : public IBaseView
{
	void data_received_();

	GstDebugger::GStreamerData::InfoTypeCase supported_info_type;
	std::string module_name;

protected:
	MainModuleModelColumns columns;

	static DetailsModelColumns detail_columns;

	Glib::RefPtr<Gtk::TreeModelFilter> filter;
	Glib::RefPtr<Gtk::ListStore> model;
	Glib::RefPtr<Gtk::ListStore> details_model;

	void append_details_row(const std::string &name, const std::string &value);
	void append_details_from_structure(Gst::Structure& structure);

	virtual void data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data) = 0;
	virtual bool filter_function(const Gtk::TreeModel::const_iterator& it) { return true; }

	virtual void load_details(gpointer data) = 0;

public:
	BaseMainModule(GstDebugger::GStreamerData::InfoTypeCase info_type, const std::string &module_name);
	virtual ~BaseMainModule() {}

	void configure_main_list_view(Gtk::TreeView *view);
	void load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path);
	virtual void details_activated(const Gtk::TreeModel::Path &path) {}

	static void configure_details_view(Gtk::TreeView *view);

	void update_filter_expression(const std::string &expr);

	Glib::RefPtr<Gtk::ListStore> get_model() const { return model; }

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_MODULES_BASE_MAIN_MODULE_H_ */
