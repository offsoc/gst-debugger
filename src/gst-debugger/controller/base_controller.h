/*
 * base_controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_BASE_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_BASE_CONTROLLER_H_

#include "tcp_client.h"

#include <memory>

class BaseController
{
protected:
	std::shared_ptr<TcpClient> client;

public:
	BaseController() : client(std::make_shared<TcpClient>()) {}
	virtual ~BaseController() {}
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_BASE_CONTROLLER_H_ */
