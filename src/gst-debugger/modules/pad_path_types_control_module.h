/*
 * pad_path_types_control_module.h
 *
 *  Created on: Sep 1, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_PAD_PATH_TYPES_CONTROL_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_PAD_PATH_TYPES_CONTROL_MODULE_H_

#include "pad_path_control_module.h"
#include "types_control_module.h"

class PadPathTypesControlModule : public PadPathControlModule, public TypesControlModule
{
	bool add_hook_unlocked() override
	{
		return PadPathControlModule::add_hook_unlocked() && TypesControlModule::add_hook_unlocked();
	}

public:
	PadPathTypesControlModule(const std::string &type_name, PadWatch_WatchType watch_type)
	: TypesControlModule(type_name, watch_type),
	  PadPathControlModule(watch_type),
	  HooksControlModule(watch_type)
	{
	}

	virtual ~PadPathTypesControlModule () {}

	Gtk::Widget* get_widget() override
	{
		if (main_box == nullptr)
		{
			main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)); // todo possibly memleak

			append_types_widgets();
			append_pad_path_widgets();
			append_hook_widgets();
		}
		return main_box;
	}

	void set_controller(const std::shared_ptr<Controller> &controller) override
	{
		PadPathControlModule::set_controller(controller);
		TypesControlModule::set_controller(controller);
	}
};

#endif /* SRC_GST_DEBUGGER_MODULES_PAD_PATH_TYPES_CONTROL_MODULE_H_ */
