/*
 * control_module.h
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_CONTROL_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_CONTROL_MODULE_H_

#include "controller/iview.h"
#include "controller/controller.h"

#include "common_model_columns.h"

#include <gtkmm/widget.h>

class ControlModule : public IBaseView
{
public:
	virtual ~ControlModule() {};

	virtual Gtk::Widget* get_widget() = 0;
};

class HooksControlModule : public ControlModule
{
	PadWatch_WatchType watch_type;

protected:
	Gtk::Box *main_box = nullptr;
	Gtk::Button *add_hook_button;

	void append_hook_widgets()
	{
		add_hook_button = Gtk::manage(new Gtk::Button("Add hook"));
		add_hook_button->signal_clicked().connect([this]{
			if ((int)watch_type == -1)  // todo it has to be fixed on design protocol level
				controller->send_message_request_command(get_type(), true);
			else
				controller->send_pad_watch_command(true, watch_type, get_pad_path(), get_type());
		});

		main_box->pack_start(*add_hook_button, false, true);
		main_box->pack_start(*Gtk::manage(new Gtk::Label("Existing hooks:")));
		Gtk::ScrolledWindow *wnd = Gtk::manage(new Gtk::ScrolledWindow);
		wnd->add(*Gtk::manage(new Gtk::TreeView()));
		main_box->pack_start(*wnd, true, true);
		main_box->pack_start(*Gtk::manage(new Gtk::Button("Remove selected hook")), false, true);

		update_add_hook();
	}

	virtual bool add_hook_unlocked() { return true; }

	void update_add_hook()
	{
		add_hook_button->set_sensitive(add_hook_unlocked());
	}

	virtual int get_type() const { return -1; }
	virtual std::string get_pad_path() const { return std::string(); }

public:
	HooksControlModule(PadWatch_WatchType watch_type)
	: watch_type(watch_type)
	{
		main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)); // todo possible memleak

		append_hook_widgets();
	}

	virtual ~HooksControlModule() {}

	Gtk::Widget* get_widget() override
	{
		return main_box;
	}
};

#endif /* SRC_GST_DEBUGGER_MODULES_CONTROL_MODULE_H_ */
