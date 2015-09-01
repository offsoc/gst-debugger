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

class LogControlModule : public ControlModule
{
	Gtk::Box *main_box;
	Gtk::CheckButton *set_watch_button;
	Gtk::CheckButton *overwrite_threshold_check_button;
	Gtk::Button *set_threshold_button;
	Gtk::Entry *threshold_entry;

public:
	LogControlModule();
	virtual ~LogControlModule() {}

	Gtk::Widget* get_widget() override;
};

#endif /* SRC_GST_DEBUGGER_MODULES_LOG_MODULE_H_ */
