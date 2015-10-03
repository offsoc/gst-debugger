/*
 * main_module.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#include "main_module.h"

//#include "pad_data_modules.h"
#include "log_module.h"
#include "message_module.h"
#include "event_module.h"
#include "query_module.h"
#include "buffer_module.h"

#include "controller/controller.h"
#include "controller/element_path_processor.h"

MainModule::MainModule(const Glib::RefPtr<Gtk::Builder> &builder)
{
	builder->get_widget("mainListTreeView", list_tree_view);
	list_tree_view->signal_row_activated().connect(sigc::mem_fun(*this, &MainModule::mainListTreeView_row_activated_cb));

	builder->get_widget("mainDetailsTreeView", details_tree_view);
	details_tree_view->signal_row_activated().connect(sigc::mem_fun(*this, &MainModule::mainDetailsTreeView_row_activated_cb));
	BaseMainModule::configure_details_view(details_tree_view);

	builder->get_widget("controllerFrame", controller_frame);

	builder->get_widget("clearMainListButton", clear_main_list_button);
	clear_main_list_button->signal_clicked().connect([this] {
		current_module->clear_model();
		current_module->get_model()->clear();
	});

	load_submodules(builder);
}

void MainModule::load_submodules(const Glib::RefPtr<Gtk::Builder>& builder)
{
	submodules["logMessages"].display_module = std::make_shared<LogModule>();
	submodules["logMessages"].control_module = std::make_shared<LogControlModule>();

	submodules["queries"].display_module = std::make_shared<QueryModule>();
	submodules["queries"].control_module = std::make_shared<QueryControlModule>();

	submodules["busMessages"].display_module = std::make_shared<MessageModule>();
	submodules["busMessages"].control_module = std::make_shared<MessageControlModule>();

	submodules["buffers"].display_module = std::make_shared<BufferModule>();
	submodules["buffers"].control_module = std::make_shared<BufferControlModule>();

	submodules["events"].display_module = std::make_shared<EventModule>();
	submodules["events"].control_module = std::make_shared<EventControlModule>();

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

	update_module(submodules["logMessages"]);
}

void MainModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);

	for (auto m : submodules)
	{
		m.second.display_module->set_controller(controller);
		m.second.control_module->set_controller(controller);
	}
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

