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

bool GstDebuggerTcpClient::connect(const std::string &address, int port)
{
	try
	{
		client = Gio::SocketClient::create();
		connection = client->connect_to_host(address, port);
		connected = true;

		reader = std::thread([this]{
			read_data();
		});
		signal_status_changed(true);
		return true;
	}
	catch (const Gio::Error &)
	{
		return false;
	}
}

void GstDebuggerTcpClient::read_data()
{
	char buffer[1024]; // todo increase max size!
	auto input_stream = connection->get_input_stream();
	while (connected)
	{
		int size = gst_debugger_protocol_utils_read_header(input_stream->gobj());

		if (size < 0)
			break;

		assert(size <= 1024);
		gst_debugger_protocol_utils_read_requested_size(input_stream->gobj(), size, buffer);

		GstreamerInfo info;
		info.ParseFromArray(buffer, size);
		signal_frame_received(info);
	}

	signal_status_changed(false);
	connected = false;
}

bool GstDebuggerTcpClient::disconnect()
{
	connected = false;
	auto ok = connection->close();
	reader.join();
	return ok;
}

void GstDebuggerTcpClient::write_data(char *data, int size)
{
	auto stream = connection->get_output_stream();
	stream->write(data, size);
}

void GstDebuggerTcpClient::send_command(const Command &cmd)
{
	if (!is_connected())
		throw Gio::Error(Gio::Error::FAILED, "No connection!");

	char buffer[4];
	auto size = cmd.ByteSize();
	gst_debugger_protocol_utils_serialize_integer64(size, buffer, 4);
	connection->get_output_stream()->write(buffer, 4);
	cmd.SerializeToFileDescriptor(connection->get_socket()->get_fd());
}
