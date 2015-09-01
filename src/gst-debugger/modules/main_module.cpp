/*
 * main_module.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#include "main_module.h"

#include "pad_data_modules.h"
#include "log_module.h"
#include "bus_messages_module.h"
#include "pad_path_types_control_module.h"

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
	builder->get_widget("controllerFrame", controller_frame);

	create_dispatcher("selected-object", sigc::mem_fun(*this, &MainModule::selected_object_changed), nullptr);

	load_submodules(builder);
}

void MainModule::load_submodules(const Glib::RefPtr<Gtk::Builder>& builder)
{
	submodules["logMessages"].display_module = std::make_shared<LogModule>();
	submodules["logMessages"].control_module = std::make_shared<LogControlModule>();

	submodules["queries"].display_module = std::make_shared<QueryModule>();
	submodules["queries"].control_module = std::make_shared<PadPathTypesControlModule>("GstQueryType", PadWatch_WatchType_QUERY);

	submodules["busMessages"].display_module = std::make_shared<BusMessagesModule>();
	submodules["busMessages"].control_module = std::make_shared<TypesControlModule>("GstMessageType", (PadWatch_WatchType)-1);

	submodules["buffers"].display_module = std::make_shared<BufferModule>();
	submodules["buffers"].control_module = std::make_shared<PadPathControlModule>(PadWatch_WatchType_BUFFER);

	submodules["events"].display_module = std::make_shared<EventModule>();
	submodules["events"].control_module = std::make_shared<PadPathTypesControlModule>("GstEventType", PadWatch_WatchType_EVENT);

	for (auto m : submodules)
	{
		builder->get_widget(m.first + "ToolButton", m.second.switch_button);
		m.second.switch_button->signal_toggled().connect([this, m] {
			if (m.second.switch_button->get_active())
			{
				update_module(m.second);
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
		m.second.display_module->set_controller(controller);
		m.second.control_module->set_controller(controller);
	}
}

void MainModule::selected_object_changed()
{
	std::shared_ptr<PadModel> selected_pad = std::dynamic_pointer_cast<PadModel>(controller->get_selected_object());
	pad_path_label->set_text(selected_pad ? ElementPathProcessor::get_object_path(selected_pad) : "(none)");
}

void MainModule::update_module(const MainModuleInfo &module_info)
{
	module_info.display_module->configure_main_list_view(list_tree_view);
	current_module = module_info.display_module;
	controller_frame->remove();
	controller_frame->add(*(module_info.control_module->get_widget()));
	controller_frame->show_all();
}

void MainModule::mainListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
	current_module->load_details(details_tree_view, path);
}

void MainModule::mainDetailsTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
	current_module->details_activated(path);
}

