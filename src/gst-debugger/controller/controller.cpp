/*
 * controller.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "controller.h"

#include <gtkmm.h>

Controller::Controller(IMainView *view)
 : view(view)
{
	client->signal_frame_received.connect(sigc::mem_fun(*this, &Controller::process_frame));
}

int Controller::run(int &argc, char **&argv)
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "gst.debugger.com");
	view->set_controller(shared_from_this());
	return app->run(*view);
}

void Controller::send_command(const Command& cmd)
{
	client->send_command(cmd);
}

void Controller::process_frame(const GstreamerInfo &info)
{
	switch (info.info_type())
	{
	case GstreamerInfo_InfoType_TOPOLOGY:
		process(info.topology());
		view->set_current_model(current_model);
		break;
	default:
		break;
	}
}
