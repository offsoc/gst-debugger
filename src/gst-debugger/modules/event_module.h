/*
 * event_module.h
 *
 *  Created on: Sep 29, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_EVENT_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_EVENT_MODULE_H_

#include "base_main_module.h"
#include "control_module.h"
#include "qe_control_module.h"

class EventModule : public BaseMainModule
{
	void data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data) override;
	void load_details(gpointer data) override;

public:
	EventModule();
	virtual ~EventModule() {}
};

class EventControlModule : virtual public ControlModule, public QEControlModule
{
	void confirmation_received(GstDebugger::Command* cmd) override;

	void add_hook() override;
	void remove_hook(const Gtk::TreeModel::Row& row) override;

	bool hook_is_the_same(const Gtk::TreeModel::Row& row, gconstpointer confirmation) override
	{
		auto pad = reinterpret_cast<const GstDebugger::PadHookRequest*>(confirmation);
		return row[hooks_model_columns.int1] == pad->event().type() && row[hooks_model_columns.str2] == pad->pad();
	}

public:
	EventControlModule();
	virtual ~EventControlModule() {}
};

#endif /* SRC_GST_DEBUGGER_MODULES_EVENT_MODULE_H_ */
