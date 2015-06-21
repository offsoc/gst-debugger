/*
 * gst_debugger_tcp_client.h
 *
 *  Created on: Jun 21, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_DEBUGGER_TCP_CLIENT_H_
#define SRC_GST_DEBUGGER_GST_DEBUGGER_TCP_CLIENT_H_

#include "protocol/gstdebugger.pb.h"

#include <giomm.h>

#include <thread>
#include <functional>

class GstDebuggerTcpClient
{
public:
	typedef std::function<void(const GstreamerInfo& info)> frame_received_function;

private:
	Glib::RefPtr<Gio::SocketClient> client;
	Glib::RefPtr<Gio::SocketConnection> connection;

	bool connected = false;
	std::thread reader;

	frame_received_function frame_received_handler;

	void read_data();

public:
	void connect(const std::string &address, int port);
	void disconnect();
	void write_data(char *data, int size);

	void set_frame_received_handler(frame_received_function handler);
};

#endif /* SRC_GST_DEBUGGER_GST_DEBUGGER_TCP_CLIENT_H_ */
