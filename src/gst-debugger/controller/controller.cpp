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
   view(view),
   client(std::make_shared<TcpClient>())
{
	view->set_controller(shared_from_this());
}

int Controller::run(int &argc, char **&argv)
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "gst.debugger.com");

	return app->run(*view);
}
