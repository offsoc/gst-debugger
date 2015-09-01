/*
 * main_module.h
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_MAIN_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_MAIN_MODULE_H_

#include "base_main_module.h"
#include "control_module.h"

#include "controller/iview.h"

#include <gtkmm.h>

class MainModule : public IBaseView
{
	struct MainModuleInfo
	{
		std::shared_ptr<BaseMainModule> display_module;
		std::shared_ptr<ControlModule> control_module;
		Gtk::RadioToolButton *switch_button;
	};

	Gtk::TreeView *list_tree_view;
	Gtk::TreeView *details_tree_view;
	Gtk::Frame *controller_frame;
	Gtk::Entry *data_filter_entry;

	TypesModelColumns types_columns;

	std::shared_ptr<BaseMainModule> current_module;

	std::map<std::string, MainModuleInfo> submodules;

	void load_submodules(const Glib::RefPtr<Gtk::Builder>& builder);
	void update_module(const MainModuleInfo &module_info);

	void mainListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);
	void mainDetailsTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);

public:
	MainModule(const Glib::RefPtr<Gtk::Builder>& builder);

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_MODULES_MAIN_MODULE_H_ */
