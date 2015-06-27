/*
 * main.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "gst-debugger.ui.h"
#include "main_window.h"

#include <gtkmm.h>
#include <gstreamermm.h>

int main(int argc, char** argv)
{
	Gst::init(argc, argv);
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "number.plate.recognition");

	Glib::RefPtr<Gtk::Builder> builder =
			Gtk::Builder::create_from_string(std::string((char*)gst_debugger_glade, gst_debugger_glade_len));

	MainWindow* wnd_handler;
	builder->get_widget_derived("mainWindow", wnd_handler);

	app->run(*wnd_handler);

	return 0;
}
