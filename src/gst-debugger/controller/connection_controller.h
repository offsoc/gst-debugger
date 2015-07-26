/*
 * connection_controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_CONNECTION_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_CONNECTION_CONTROLLER_H_

#include "base_controller.h"

class ConnectionController : virtual public BaseController
{
public:
	bool is_connected() { return client->is_connected(); }

	void disconnect() { client->disconnect(); }

	void connect (const std::string &ip_address, int port) { client->connect(ip_address, port); }

	void on_connection_status_changed(const sigc::signal1<void, bool>::slot_type& slot)
	{
		client->signal_status_changed.connect(slot);
	}

};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_CONNECTION_CONTROLLER_H_ */
