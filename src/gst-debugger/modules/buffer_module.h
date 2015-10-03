/*
 * buffer_module.h
 *
 *  Created on: Oct 3, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_BUFFER_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_BUFFER_MODULE_H_

#include "control_module.h"
#include "base_main_module.h"

class BufferModule : public BaseMainModule
{
	void data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data) override;
	void load_details(gpointer data) override;

public:
	BufferModule();
	virtual ~BufferModule() {}
};

class BufferControlModule : public ControlModule
{
	Gtk::Label *pad_path_label;
	Gtk::CheckButton *data_check_button;

	void add_watch() override;
	void remove_watch(const Gtk::TreeModel::Row& row) override;
	void confirmation_received(GstDebugger::Command* cmd) override;

	bool hook_is_the_same(const Gtk::TreeModel::Row& row, gconstpointer confirmation) override
	{
		auto buffer = reinterpret_cast<const GstDebugger::PadWatchRequest*>(confirmation);
		return row[hooks_model_columns.str1] == buffer->pad();
	}

public:
	BufferControlModule();
	virtual ~BufferControlModule() {}
};

#endif /* SRC_GST_DEBUGGER_MODULES_BUFFER_MODULE_H_ */
