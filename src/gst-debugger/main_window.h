/*
 * main_window.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_MAIN_WINDOW_H_
#define SRC_GST_DEBUGGER_MAIN_WINDOW_H_

#include "gst_debugger_tcp_client.h"
#include "connection_properties_dialog.h"

#include <gtkmm.h>

class MainWindow : public Gtk::Window
{
	void connectionPropertiesMenuItem_activate_cb();
	void connectMenuItem_activate_cb();
	void connection_status_changed(bool connected);

	GstDebuggerTcpClient client;

	Glib::RefPtr<Gtk::Builder> builder;
	Gtk::MenuItem *connection_properties;
	Gtk::ImageMenuItem *connect_menu_item;
	ConnectionPropertiesDialog *connection_properties_dialog;
	Gtk::Statusbar *main_statusbar;

public:
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~MainWindow() {}
};

#endif /* SRC_GST_DEBUGGER_MAIN_WINDOW_H_ */
