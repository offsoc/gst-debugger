/*
 * log_module.h
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_LOG_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_LOG_MODULE_H_

#include "base_main_module.h"
#include "control_module.h"

class LogModule : public BaseMainModule
{
	void data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data) override;
	void load_details(gpointer data) override;

protected:
	bool filter_function(const Gtk::TreeModel::const_iterator& it) override;

public:
	LogModule();
	virtual ~LogModule() {}
};

class LogControlModule : public ControlModule
{
	Gtk::CheckButton *overwrite_threshold_check_button;
	Gtk::Entry *log_level_entry;
	Gtk::ComboBoxText *debug_categories_combobox;
	Gtk::Button *set_threshold_button;
	Gtk::Entry *threshold_entry;

	void add_watch() override;
	void remove_watch(const Gtk::TreeModel::Row& row) override;
	void confirmation_received(GstDebugger::Command* cmd) override;

	bool hook_is_the_same(const Gtk::TreeModel::Row& row, gconstpointer confirmation) override
	{
		auto log = reinterpret_cast<const GstDebugger::LogRequest*>(confirmation);
		return row[hooks_model_columns.int1] == log->level() &&
			row[hooks_model_columns.str2] == log->category();
	}

public:
	LogControlModule();
	virtual ~LogControlModule() {}

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_MODULES_LOG_MODULE_H_ */
