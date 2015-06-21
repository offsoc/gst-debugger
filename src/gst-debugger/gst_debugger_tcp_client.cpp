/*
 * gst_debugger_tcp_client.cpp
 *
 *  Created on: Jun 21, 2015
 *      Author: mkolny
 */

#include "gst_debugger_tcp_client.h"

extern "C" {
#include "protocol/protocol_utils.h"
}

#include <cassert>

void GstDebuggerTcpClient::connect(const std::string &address, int port)
{
	client = Gio::SocketClient::create();
	connection = client->connect_to_host(address, port);
	connected = true;

	reader = std::thread([this]{
		read_data();
	});
}

void GstDebuggerTcpClient::set_frame_received_handler(frame_received_function handler)
{
	frame_received_handler = handler;
}

void GstDebuggerTcpClient::read_data()
{
	char buffer[1024]; // todo increase max size!
	auto input_stream = connection->get_input_stream();
	while (connected)
	{
		int size = gst_debugger_protocol_utils_read_header(input_stream->gobj());
		assert(size <= 1024);
		gst_debugger_protocol_utils_read_requested_size(input_stream->gobj(), size, buffer);

		GstreamerInfo info;
		info.ParseFromArray(buffer, size);
		frame_received_handler(info);
	}
}

void GstDebuggerTcpClient::disconnect()
{
	connected = false;
	connection->close();
	reader.join();
}

void GstDebuggerTcpClient::write_data(char *data, int size)
{
	auto stream = connection->get_output_stream();
	stream->write(data, size);
}
