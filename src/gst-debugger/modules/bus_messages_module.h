/*
 * bus_messages_module.h
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_BUS_MESSAGES_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_BUS_MESSAGES_MODULE_H_

#include "base_main_module.h"

class GstreamerLog;

class BusMessageModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	BusMessageModelColumns() {
		add(header); add(message);
	}

	Gtk::TreeModelColumn<Glib::ustring> header;
	Gtk::TreeModelColumn<GstMessage*> message;
};

class BusMessagesModule : public BaseMainModule
{
	void bus_message_received_();

	BusMessageModelColumns columns;

public:
	BusMessagesModule();
	virtual ~BusMessagesModule() {}

	void configure_main_list_view(Gtk::TreeView *view) override;
	void load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path) override;

	void set_controller(const std::shared_ptr<Controller> &controller) override;

	bool filter_function(const Gtk::TreeModel::const_iterator& it) override;
};

#endif /* SRC_GST_DEBUGGER_MODULES_BUS_MESSAGES_MODULE_H_ */
