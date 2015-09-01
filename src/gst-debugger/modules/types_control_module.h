/*
 * types_control_module.h
 *
 *  Created on: Sep 1, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_TYPES_CONTROL_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_TYPES_CONTROL_MODULE_H_

#include "control_module.h"

class TypesControlModule : virtual public HooksControlModule
{
protected:
	TypesModelColumns types_model_columns;
	Glib::RefPtr<Gtk::ListStore> types_model;
	std::string type_name;

	Gtk::CheckButton *any_type_check_button;
	Gtk::ComboBox *types_combobox;

	void append_types_widgets();

	void enum_list_changed_();

	bool add_hook_unlocked() override;

	int get_type() const override;

public:
	TypesControlModule(const std::string &enum_type_name, PadWatch_WatchType watch_type);
	virtual ~TypesControlModule() {}

	Gtk::Widget* get_widget() override;

	void set_controller(const std::shared_ptr<Controller> &controller) override;

};

#endif /* SRC_GST_DEBUGGER_MODULES_TYPES_CONTROL_MODULE_H_ */
