/*
 * base_controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_BASE_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_BASE_CONTROLLER_H_

#include "tcp_client.h"
#include "models/gst_pipeline_model.h"

#include <memory>

class BaseController
{
protected:
	std::shared_ptr<TcpClient> client;
	std::shared_ptr<ElementModel> current_model;

public:
	BaseController() : client(std::make_shared<TcpClient>()), current_model(ElementModel::get_root()) {}
	virtual ~BaseController() {}
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_BASE_CONTROLLER_H_ */
