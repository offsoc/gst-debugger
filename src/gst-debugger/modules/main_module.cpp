/*
 * main_module.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#include "main_module.h"

#include "pad_data_modules.h"
#include "log_module.h"

#include "controller/controller.h"
#include "controller/element_path_processor.h"

MainModule::MainModule(const Glib::RefPtr<Gtk::Builder> &builder)
{
	builder->get_widget("mainListTreeView", list_tree_view);
	list_tree_view->signal_row_activated().connect(sigc::mem_fun(*this, &MainModule::mainListTreeView_row_activated_cb));

	builder->get_widget("mainDetailsTreeView", details_tree_view);
	details_tree_view->signal_row_activated().connect(sigc::mem_fun(*this, &MainModule::mainDetailsTreeView_row_activated_cb));
	BaseMainModule::configure_details_view(details_tree_view);

	builder->get_widget("existingHooksTreeView", existing_hooks_tree_view);
	builder->get_widget("hookPadPathLabel", pad_path_label);

	builder->get_widget("addNewHookButton", add_hook_button);
	builder->get_widget("removeSelectedHook", remove_selected_hook_button);

	builder->get_widget("hookTypesComboBox", types_combobox);
	builder->get_widget("hookTypeBox", hook_type_box);
	builder->get_widget("hookAnyPathCheckButton", any_path_check_button);
	builder->get_widget("hookAnyTypeCheckButton", any_type_check_button);

	create_dispatcher("selected-object", sigc::mem_fun(*this, &MainModule::selected_object_changed), nullptr);

	load_submodules(builder);
}

void MainModule::load_submodules(const Glib::RefPtr<Gtk::Builder>& builder)
{
	submodules["logMessages"].module = std::make_shared<LogModule>();
	submodules["queries"].module = std::make_shared<QueryModule>();
	//main_modules["busMessages"].module = std::make_shared<GstMessageModule>();
	submodules["buffers"].module = std::make_shared<BufferModule>();
	submodules["events"].module = std::make_shared<EventModule>();

	for (auto m : submodules)
	{
		builder->get_widget(m.first + "ToolButton", m.second.switch_button);
		m.second.switch_button->signal_toggled().connect([this, m] {
			if (m.second.switch_button->get_active())
			{
				update_module(m.second.module);
			}
		});
	}
}

void MainModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);

	controller->on_selected_object_changed.connect([this]{gui_emit("selected-object");});

	for (auto m : submodules)
	{
		m.second.module->set_controller(controller);
	}
}

void MainModule::selected_object_changed()
{
	std::shared_ptr<PadModel> selected_pad = std::dynamic_pointer_cast<PadModel>(controller->get_selected_object());
	pad_path_label->set_text(selected_pad ? ElementPathProcessor::get_object_path(selected_pad) : "(none)");
}

void MainModule::update_module(const std::shared_ptr<BaseMainModule> &module)
{
	module->configure_main_list_view(list_tree_view);
	current_module = module;
}

void MainModule::mainListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
	current_module->load_details(details_tree_view, path);
}

void MainModule::mainDetailsTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
	current_module->details_activated(path);
}

