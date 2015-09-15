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
#include "filter-parser/parser.h"

#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>

class BaseMainModule : public IBaseView
{
protected:
	std::shared_ptr<Expression> filter_expression;

	static DetailsModelColumns detail_columns;

	Glib::RefPtr<Gtk::TreeModelFilter> filter;
	Glib::RefPtr<Gtk::ListStore> model;
	Glib::RefPtr<Gtk::ListStore> details_model;

	void append_details_row(const std::string &name, const std::string &value);
	void append_details_from_structure(Gst::Structure& structure);

	virtual bool filter_function(const Gtk::TreeModel::const_iterator& it) { return true; }

public:
	BaseMainModule();
	virtual ~BaseMainModule() {}

	virtual void configure_main_list_view(Gtk::TreeView *view) = 0;
	virtual void load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path) = 0;
	virtual void details_activated(const Gtk::TreeModel::Path &path) {}

	static void configure_details_view(Gtk::TreeView *view);

	void update_filter_expression(const std::string &expr);

	Glib::RefPtr<Gtk::ListStore> get_model() const { return model; }
};

#endif /* SRC_GST_DEBUGGER_MODULES_BASE_MAIN_MODULE_H_ */
