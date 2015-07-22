/*
 * gst_debugger_tcp_client.h
 *
 *  Created on: Jun 21, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_DEBUGGER_TCP_CLIENT_H_
#define SRC_GST_DEBUGGER_GST_DEBUGGER_TCP_CLIENT_H_

#include "protocol/gstdebugger.pb.h"

#include <sigc++/sigc++.h>
#include <giomm.h>

#include <thread>
#include <functional>

class TcpClient
{
public:
	typedef sigc::signal1<void, const GstreamerInfo&> frame_received_slot;

private:
	Glib::RefPtr<Gio::SocketClient> client;
	Glib::RefPtr<Gio::SocketConnection> connection;

	bool connected = false;
	std::thread reader;

	void read_data();

public:
	bool connect(const std::string &address, int port);
	bool disconnect();
	void write_data(char *data, int size);
	void send_command(const Command &cmd);

	bool is_connected() const { return connection && connection->is_connected(); }

	sigc::signal1<void, bool> signal_status_changed;
	frame_received_slot signal_frame_received;
};

#endif /* SRC_GST_DEBUGGER_GST_DEBUGGER_TCP_CLIENT_H_ */
