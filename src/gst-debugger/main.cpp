/*
 * main.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "gst-debugger-resources.h"
#include "controller/controller.h"
#include "main_window.h"

#include "config.h"

#include <glibmm/i18n.h>
#include <gtkmm.h>

int main(int argc, char** argv)
{

	bindtextdomain(GETTEXT_PACKAGE, GST_DEBUGGER_LOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	Gst::init(argc, argv);
	Gtk::Main kit(argc, argv);
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_resource("/eu/cookandcommit/gst-debugger/ui/gst-debugger.glade");
	MainWindow* wnd_handler;
	builder->get_widget_derived("mainWindow", wnd_handler);

	std::shared_ptr<Controller> controller(new Controller(wnd_handler));

	controller->run(argc, argv);

	return 0;
}
