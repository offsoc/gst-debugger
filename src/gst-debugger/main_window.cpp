/*
 * main_window.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "main_window.h"
#include "sigc++lambdahack.h"

#include "gst-debugger-dialogs.ui.h"
#include "gst-debugger-logo.h"

#include "controller/controller.h"

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: IMainView(cobject),
  builder(builder),
  dispatcher(std::make_shared<Glib::Dispatcher>()),
  log_module(std::make_shared<GstLogModule>(builder)),
  event_module(std::make_shared<GstEventModule>(builder)),
  query_module(std::make_shared<GstQueryModule>(builder)),
  message_module(std::make_shared<GstMessageModule>(builder)),
  buffer_module(std::make_shared<GstBufferModule>(builder)),
  properties_module(std::make_shared<GstPropertiesModule>(builder))
{
	graph_module = std::make_shared<GraphModule>(builder);

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

	enums_dialog = load_dialog<EnumsDialog>("remoteDataDialog");
	factories_dialog = load_dialog<FactoriesDialog>("remoteDataDialog");
	connection_properties_dialog = load_dialog<ConnectionPropertiesDialog>("connectionPropertiesDialog");

	{
		Glib::RefPtr<Gtk::Builder> dialogs_builder = get_dialog_ui_def();
		dialogs_builder->get_widget("aboutDialog", about_dialog);
		// todo use gio::resources
		about_dialog->set_logo(Gdk::Pixbuf::create_from_inline(429324+24, gst_debugger_logo , false));
		about_dialog->set_transient_for(*this);
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

	log_module->set_controller(controller);
	event_module->set_controller(controller);
	query_module->set_controller(controller);
	message_module->set_controller(controller);
	buffer_module->set_controller(controller);
	graph_module->set_controller(controller);
	properties_module->set_controller(controller);

	enums_dialog->set_controller(controller);
	enums_dialog->set_transient_for(*this);

	factories_dialog->set_controller(controller);
	factories_dialog->set_transient_for(*this);

	connection_properties_dialog->set_transient_for(*this);
}

Glib::RefPtr<Gtk::Builder> MainWindow::get_dialog_ui_def()
{
	return Gtk::Builder::create_from_string(std::string((char*)gst_debugger_dialogs_glade, gst_debugger_dialogs_glade_len));
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

