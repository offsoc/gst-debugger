/*
 * controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_

#include "models/gst_pipeline_model.h"

#include "iview.h"
#include "tcp_client.h"

class Controller : public std::enable_shared_from_this<Controller>
{
private:
	std::shared_ptr<ElementModel> current_model;
	IView *view;
	std::shared_ptr<TcpClient> client;

public:
	Controller(IView *view);

	int run(int &argc, char **&argv);
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_ */