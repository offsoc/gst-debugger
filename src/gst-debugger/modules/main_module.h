/*
 * main_module.h
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_MAIN_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_MAIN_MODULE_H_

#include "controller/iview.h"

#include "base_main_module.h"

#include <gtkmm.h>

class MainModule : public IBaseView
{
	struct MainModuleInfo
	{
		std::shared_ptr<BaseMainModule> module;
		Gtk::RadioToolButton *switch_button;
	};

	Gtk::TreeView *list_tree_view;
	Gtk::TreeView *details_tree_view;
	Gtk::Button *add_hook_button;
	Gtk::Button *remove_selected_hook_button;
	Gtk::ComboBox *types_combobox;
	Gtk::CheckButton *any_path_check_button;
	Gtk::CheckButton *any_type_check_button;
	Gtk::TreeView *existing_hooks_tree_view;
	Gtk::Label *pad_path_label;
	Gtk::Box *hook_type_box;

	TypesModelColumns types_columns;

	std::shared_ptr<BaseMainModule> current_module;

	std::map<std::string, MainModuleInfo> submodules;

	void selected_object_changed();
	void load_submodules(const Glib::RefPtr<Gtk::Builder>& builder);

	void mainListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);
	void mainDetailsTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);

public:
	MainModule(const Glib::RefPtr<Gtk::Builder>& builder);

	void set_controller(const std::shared_ptr<Controller> &controller) override;

	void update_module(const std::shared_ptr<BaseMainModule> &module);
};

#endif /* SRC_GST_DEBUGGER_MODULES_MAIN_MODULE_H_ */
