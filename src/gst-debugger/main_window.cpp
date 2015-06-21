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

	builder->get_widget("logThresholdEntry", log_threshold_entry);
	builder->get_widget("overwriteCurrentThresholdCheckButton", overwrite_current_threshold_check_button);

	builder->get_widget("setThresholdButton", set_threshold_button);
	set_threshold_button->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::setThresholdButton_clicked_cb));

	builder->get_widget_derived("connectionPropertiesDialog", connection_properties_dialog);
	builder->get_widget("mainStatusbar", main_statusbar);

	builder->get_widget("watchLogCheckButton", watch_log_check_button);
	watch_log_check_button->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::watchLogCheckButton_toggled_cb));

	client.signal_status_changed.connect(sigc::mem_fun(*this, &MainWindow::connection_status_changed));
	client.signal_frame_received.connect([](const GstreamerInfo &info){
		// todo
	});
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

void MainWindow::setThresholdButton_clicked_cb()
{
	Command cmd;
	LogThreshold *log_threshold = new LogThreshold();
	log_threshold->set_list(log_threshold_entry->get_text());
	log_threshold->set_overwrite(overwrite_current_threshold_check_button->get_active());
	cmd.set_command_type(Command_CommandType_LOG_THRESHOLD);
	cmd.set_allocated_log_threshold(log_threshold);
	client.send_command(cmd);
}

void MainWindow::watchLogCheckButton_toggled_cb()
{
	Command cmd;
	LogWatch *log_watch = new LogWatch();
	log_watch->set_toggle(watch_log_check_button->get_active() ? ENABLE : DISABLE);
	log_watch->set_log_level(10); // todo
	cmd.set_command_type(Command_CommandType_LOG_WATCH);
	cmd.set_allocated_log_watch(log_watch);
	client.send_command(cmd);
}
