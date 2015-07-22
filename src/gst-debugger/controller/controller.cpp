/*
 * controller.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "controller.h"

#include <gtkmm.h>

Controller::Controller(IView *view)
 : current_model(ElementModel::get_root()),
   view(view)
{
	view->set_controller(shared_from_this());
}

int Controller::run(int &argc, char **&argv)
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "gst.debugger.com");

	return app->run(*view);
}

void Controller::send_command(const Command& cmd)
{
	client->send_command(cmd);
}
