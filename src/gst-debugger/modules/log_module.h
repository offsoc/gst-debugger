/*
 * log_module.h
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_LOG_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_LOG_MODULE_H_

#include "base_main_module.h"

class GstreamerLog;

class LogModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	LogModelColumns() {
		add(header); add(log);
	}

	Gtk::TreeModelColumn<Glib::ustring> header;
	Gtk::TreeModelColumn<GstreamerLog*> log;
};

class LogModule : public BaseMainModule
{
	void log_received_();

	LogModelColumns columns;

public:
	LogModule();
	virtual ~LogModule() {}

	void configure_main_list_view(Gtk::TreeView *view) override;
	void load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path) override;

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};


#endif /* SRC_GST_DEBUGGER_MODULES_LOG_MODULE_H_ */
