/*
 * main_window.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "config.h"

#include "main_window.h"
#include "ui_utils.h"

#include "gst-debugger-resources.h"

#include "controller/controller.h"

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: IMainView(cobject),
  builder(builder),
  dispatcher(std::make_shared<Glib::Dispatcher>()),
  main_module(std::make_shared<MainModule>(builder)),
  graph_module(std::make_shared<GraphModule>(builder)),
  properties_module(std::make_shared<GstPropertiesModule>(builder))
{
	builder->get_widget("connectionPropertiesMenuItem", connection_properties);
	connection_properties->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::connectionPropertiesMenuItem_activate_cb));

	builder->get_widget("remoteEnumTypesMenuitem", remote_enum_types);
	remote_enum_types->signal_activate().connect([this] { enums_dialog->show(); });

	builder->get_widget("remoteFactoriesMenuitem", remote_factories);
	remote_factories->signal_activate().connect([this] { factories_dialog->show(); });

	builder->get_widget("connectMenuItem", connect_menu_item);
	connect_menu_item->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::connectMenuItem_activate_cb));

	builder->get_widget("aboutMenuItem", about_menu_item);
	about_menu_item->signal_activate().connect([this] { about_dialog->show(); });

	builder->get_widget("mainStatusbar", main_statusbar);

	builder->get_widget("quitMenuItem", quit_menu_item);
	quit_menu_item->signal_activate().connect([this] { this->close(); });

	enums_dialog = load_dialog<EnumsDialog>("remoteDataDialog");
	factories_dialog = load_dialog<FactoriesDialog>("remoteDataDialog");
	connection_properties_dialog = load_dialog<ConnectionPropertiesDialog>("connectionPropertiesDialog");

	{
		Glib::RefPtr<Gtk::Builder> dialogs_builder = get_dialog_ui_def();
		dialogs_builder->get_widget("aboutDialog", about_dialog);
		about_dialog->set_logo(Gdk::Pixbuf::create_from_resource("/eu/cookandcommit/gst-debugger/ui/gst-debugger-logo.png"));
		about_dialog->set_transient_for(*this);
		about_dialog->set_version(VERSION);
	}

	builder->get_widget("mainStatusbar", main_statusbar);

	connection_status_changed(false);

	signal_show().connect([this] {
		graph_module->update_model_();
	});

	signal_delete_event().connect([this](GdkEventAny*){
		graph_module->free_graph();
		return false;
	});
}

void MainWindow::set_controller(const std::shared_ptr<Controller> &controller)
{
	this->controller = controller;

	controller->on_connection_status_changed(sigc::mem_fun(*this, &MainWindow::connection_status_changed));

	main_module->set_controller(controller);
	graph_module->set_controller(controller);

	enums_dialog->set_controller(controller);
	enums_dialog->set_transient_for(*this);

	factories_dialog->set_controller(controller);
	factories_dialog->set_transient_for(*this);

	properties_module->set_controller(controller);

	connection_properties_dialog->set_transient_for(*this);
}

void MainWindow::connectionPropertiesMenuItem_activate_cb()
{
	connection_properties_dialog->show();
}

void MainWindow::connectMenuItem_activate_cb()
{
	if (controller->is_connected())
		controller->disconnect();
	else
		controller->connect(
			connection_properties_dialog->get_ip_address(),
			connection_properties_dialog->get_port());
}

void MainWindow::connection_status_changed(bool connected)
{
	auto id = main_statusbar->get_context_id("connection-status");
	if (connected)
	{
		main_statusbar->push("Connected", id);
		((Gtk::Label*)connect_menu_item->get_child())->set_text("Disconnect");
	}
	else
	{
		main_statusbar->push("Disconnected", id);
		((Gtk::Label*)connect_menu_item->get_child())->set_text("Connect");
	}
}
