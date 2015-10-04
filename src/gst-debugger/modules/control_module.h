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

class HooksModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	HooksModelColumns() {
		add(str1); add(str2); add(int1); add(int2);
	}

	Gtk::TreeModelColumn<Glib::ustring> str1;
	Gtk::TreeModelColumn<Glib::ustring> str2;
	Gtk::TreeModelColumn<gint> int1;
	Gtk::TreeModelColumn<gint> int2;
};

inline void free_confirmation(GstDebugger::Command *cmd) { delete cmd; }

class ControlModule : public IBaseView
{
protected:
	Glib::RefPtr<Gtk::ListStore> hooks_model;
	HooksModelColumns hooks_model_columns;

	Gtk::Box *main_box;
	Gtk::Button *add_hook_button;
	Gtk::Button *remove_hook_button;
	Gtk::TreeView *hooks_tree_view;
	Gtk::ScrolledWindow *wnd;

	void confirmation_()
	{
		auto confirmation = gui_pop<GstDebugger::Command*>("confirmation");
		confirmation_received(confirmation);
		delete confirmation;
	}

	template<typename T>
	void remove_confirmation_hook(const T& confirmation)
	{
		for (auto iter = hooks_model->children().begin();
				iter != hooks_model->children().end(); ++iter)
		{
			if (hook_is_the_same(*iter, &confirmation))
			{
				hooks_model->erase(iter);
				break;
			}
		}
	}

	virtual bool hook_is_the_same(const Gtk::TreeModel::Row& row, gconstpointer confirmation) = 0;
	virtual void add_hook() {}
	virtual void remove_hook(const Gtk::TreeModel::Row& row) {}
	virtual void confirmation_received(GstDebugger::Command* cmd) {}

public:
	ControlModule()
	{
		main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

		add_hook_button = Gtk::manage(new Gtk::Button("Add hook"));
		add_hook_button->signal_clicked().connect([this] {
			add_hook();
		});
		main_box->pack_start(*add_hook_button, false, true);

		hooks_tree_view = Gtk::manage(new Gtk::TreeView());
		wnd = Gtk::manage(new Gtk::ScrolledWindow);
		wnd->add(*hooks_tree_view);
		main_box->pack_start(*wnd, true, true);
		hooks_model = Gtk::ListStore::create(hooks_model_columns);
		hooks_tree_view->set_model(hooks_model);

		remove_hook_button = Gtk::manage(new Gtk::Button("Remove hook"));
		main_box->pack_start(*remove_hook_button, false, true);
		remove_hook_button->signal_clicked().connect([this]{
			auto selection = hooks_tree_view->get_selection();
			if (!selection) return;
			auto iter = selection->get_selected();
			if (!iter) return;
			remove_hook(*iter);
		});

		create_dispatcher("confirmation", sigc::mem_fun(*this, &ControlModule::confirmation_), (GDestroyNotify)free_confirmation);
	}

	virtual ~ControlModule() {};

	Gtk::Widget* get_widget()
	{
		return main_box;
	}

	void set_controller(const std::shared_ptr<Controller> &controller)
	{
		IBaseView::set_controller(controller);

			controller->on_confirmation_received.connect([this](const GstDebugger::Command& command) {

			gui_push("confirmation", new GstDebugger::Command(command));
			gui_emit("confirmation");
		});
	}
};

#endif /* SRC_GST_DEBUGGER_MODULES_CONTROL_MODULE_H_ */
