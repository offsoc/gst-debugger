/*
 * query_module.h
 *
 *  Created on: Sep 29, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_QUERY_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_QUERY_MODULE_H_

#include "base_main_module.h"
#include "control_module.h"
#include "qe_control_module.h"

class QueryModule : public BaseMainModule
{
	void data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data) override;
	void load_details(gpointer data) override;

public:
	QueryModule();
	virtual ~QueryModule() {}
};

class QueryControlModule : virtual public ControlModule, public QEControlModule
{
	void confirmation_received(GstDebugger::Command* cmd) override;

	void add_hook() override;
	void remove_hook(const Gtk::TreeModel::Row& row) override;

public:
	QueryControlModule();
	virtual ~QueryControlModule() {}
};

#endif /* SRC_GST_DEBUGGER_MODULES_QUERY_MODULE_H_ */
