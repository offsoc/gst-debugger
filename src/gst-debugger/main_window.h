/*
 * main_window.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_MAIN_WINDOW_H_
#define SRC_GST_DEBUGGER_MAIN_WINDOW_H_

#include <gtkmm.h>

class MainWindow : public Gtk::Window
{
	void connectionPropertiesMenuItem_activate_cb();

	Glib::RefPtr<Gtk::Builder> builder;
	Gtk::MenuItem *connection_properties;

public:
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~MainWindow() {}
};

#endif /* SRC_GST_DEBUGGER_MAIN_WINDOW_H_ */
