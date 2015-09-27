/*
 * message_module.h
 *
 *  Created on: Sep 27, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_MESSAGE_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_MESSAGE_MODULE_H_

#include "base_main_module.h"
#include "control_module.h"

class MessageModule : public BaseMainModule
{
	void data_received(const Gtk::TreeModel::Row& row, GstDebugger::GStreamerData *data) override;
	void load_details(gpointer data) override;

public:
	MessageModule();
	virtual ~MessageModule() {}
};

class MessageControlModule : public ControlModule
{
	TypesModelColumns types_model_columns;
	Glib::RefPtr<Gtk::ListStore> types_model;
	std::string type_name;

	Gtk::ComboBox *types_combobox;

	void confirmation_received(GstDebugger::Command* cmd) override;

	void add_watch() override;
	void remove_watch(const Gtk::TreeModel::Row& row) override;

	bool hook_is_the_same(const Gtk::TreeModel::Row& row, gconstpointer confirmation) override
	{
		auto message = reinterpret_cast<const GstDebugger::MessageRequest*>(confirmation);
		return row[hooks_model_columns.int1] == message->type();
	}

public:
	MessageControlModule();
	virtual ~MessageControlModule() {}

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_MODULES_MESSAGE_MODULE_H_ */
