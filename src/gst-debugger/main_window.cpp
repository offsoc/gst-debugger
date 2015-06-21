/*
 * main_window.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "main_window.h"

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Window(cobject),
  builder(builder)
{
	builder->get_widget("connectionPropertiesMenuItem", connection_properties);
	connection_properties->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::connectionPropertiesMenuItem_activate_cb));

	builder->get_widget("connectMenuItem", connect_menu_item);
	connect_menu_item->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::connectMenuItem_activate_cb));

	builder->get_widget_derived("connectionPropertiesDialog", connection_properties_dialog);
	builder->get_widget("mainStatusbar", main_statusbar);

	client.signal_status_changed.connect(sigc::mem_fun(*this, &MainWindow::connection_status_changed));
	connection_status_changed(false);
}

void MainWindow::connectionPropertiesMenuItem_activate_cb()
{
	connection_properties_dialog->show();
}

void MainWindow::connectMenuItem_activate_cb()
{
	if (client.is_connected())
		client.disconnect();
	else
		client.connect(
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
