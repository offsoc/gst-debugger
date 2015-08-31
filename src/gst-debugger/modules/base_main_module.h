/*
 * base_main_module.h
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_BASE_MAIN_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_BASE_MAIN_MODULE_H_

#include "controller/iview.h"
#include "common_model_columns.h"

#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>

class BaseMainModule : public IBaseView
{
protected:
	static DetailsModelColumns detail_columns;

	Glib::RefPtr<Gtk::ListStore> model;
	Glib::RefPtr<Gtk::ListStore> details_model;

	void append_details_row(const std::string &name, const std::string &value);

public:
	BaseMainModule();
	virtual ~BaseMainModule() {}

	virtual void configure_main_list_view(Gtk::TreeView *view) = 0;
	virtual void load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path) = 0;
	virtual void details_activated(const Gtk::TreeModel::Path &path) {}

	static void configure_details_view(Gtk::TreeView *view);

	Glib::RefPtr<Gtk::ListStore> get_model() const { return model; }
};

#endif /* SRC_GST_DEBUGGER_MODULES_BASE_MAIN_MODULE_H_ */
