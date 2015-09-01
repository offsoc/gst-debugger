/*
 * pad_path_control_module.h
 *
 *  Created on: Sep 1, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_PAD_PATH_CONTROL_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_PAD_PATH_CONTROL_MODULE_H_

#include "control_module.h"

class PadPathControlModule : virtual public HooksControlModule
{
protected:
	Gtk::CheckButton *any_path_check_button;

	void append_pad_path_widgets();

	void selected_object_changed_();

	bool add_hook_unlocked() override;

	std::string get_pad_path() const override;

public:
	PadPathControlModule(PadWatch_WatchType watch_type);
	virtual ~PadPathControlModule() {}

	Gtk::Widget* get_widget() override;

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_MODULES_PAD_PATH_CONTROL_MODULE_H_ */
