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

class HooksModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	HooksModelColumns() {
		add(pad_path); add(qe_type_name); add(qe_type);
	}

	Gtk::TreeModelColumn<Glib::ustring> pad_path;
	Gtk::TreeModelColumn<Glib::ustring> qe_type_name;
	Gtk::TreeModelColumn<gint> qe_type;
};


class HooksControlModule : public ControlModule
{
	Gtk::ScrolledWindow *wnd;

protected:
	PadWatch_WatchType watch_type;

	Glib::RefPtr<Gtk::ListStore> hooks_model;
	HooksModelColumns hooks_model_columns;

	Gtk::TreeView *hooks_tree_view;
	Gtk::Box *main_box = nullptr;
	Gtk::Button *remove_hook_button;
	Gtk::Button *add_hook_button;

	void append_hook_widgets()
	{
		main_box->pack_start(*add_hook_button, false, true);
		main_box->pack_start(*Gtk::manage(new Gtk::Label("Existing hooks:")), false, true);

		main_box->pack_start(*wnd, true, true);
		main_box->pack_start(*remove_hook_button, false, true);

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
	HooksControlModule(PadWatch_WatchType w_type)
	: watch_type(w_type)
	{
		add_hook_button = Gtk::manage(new Gtk::Button("Add hook"));
		add_hook_button->signal_clicked().connect([this]{
			if ((int)watch_type == -1)  // todo it has to be fixed on design protocol level
				controller->send_message_request_command(get_type(), true);
			else
				controller->send_pad_watch_command(true, watch_type, get_pad_path(), get_type());
		});

		wnd = Gtk::manage(new Gtk::ScrolledWindow);

		hooks_tree_view = Gtk::manage(new Gtk::TreeView());
		wnd->add(*hooks_tree_view);
		hooks_model = Gtk::ListStore::create(hooks_model_columns);
		hooks_tree_view->set_model(hooks_model);

		remove_hook_button = Gtk::manage(new Gtk::Button("Remove selected hook"));
		remove_hook_button->signal_clicked().connect([this]{
			auto selection = hooks_tree_view->get_selection();
			if (!selection) return;
			auto iter = selection->get_selected();
			if (!iter) return;
			Gtk::TreeModel::Row row = *iter;
			auto type = row[hooks_model_columns.qe_type];
			auto pad_path = (Glib::ustring)row[hooks_model_columns.pad_path];

			if ((int)watch_type == -1)  // todo
				controller->send_message_request_command(type, false);
			else
				controller->send_pad_watch_command(false, watch_type, pad_path, type);
		});
	}

	virtual ~HooksControlModule() {}

	Gtk::Widget* get_widget() override
	{
		if (main_box == nullptr)
		{
			main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)); // todo possible memleak

			append_hook_widgets();
		}
		return main_box;
	}
};

#endif /* SRC_GST_DEBUGGER_MODULES_CONTROL_MODULE_H_ */
