/*
 * main_window.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "main_window.h"
#include "connection_properties_dialog.h"

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Window(cobject),
  builder(builder)
{
	builder->get_widget("connectionPropertiesMenuItem", connection_properties);
	connection_properties->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::connectionPropertiesMenuItem_activate_cb));
}

void MainWindow::connectionPropertiesMenuItem_activate_cb()
{
	ConnectionPropertiesDialog *dlg;
	builder->get_widget_derived("connectionPropertiesDialog", dlg);
	dlg->show();
}

