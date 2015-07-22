/*
 * controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_

#include "command_factory.h"
#include "iview.h"
#include "connection_controller.h"

#include "models/gst_pipeline_model.h"

#include "protocol/gstdebugger.pb.h"

class Controller : public std::enable_shared_from_this<Controller>, public CommandFactory, public ConnectionController
{
private:
	std::shared_ptr<ElementModel> current_model;
	IMainView *view;

public:
	Controller(IMainView *view);

	void send_command(const Command& cmd);

	int run(int &argc, char **&argv);
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_ */
