/*
 * gst_debugger_tcp_client.cpp
 *
 *  Created on: Jun 21, 2015
 *      Author: mkolny
 */

#include "tcp_client.h"

#include "common/protocol_utils.h"

#include <cassert>

bool TcpClient::connect(const std::string &address, int port)
{
	try
	{
		cancel = Gio::Cancellable::create();
		client = Gio::SocketClient::create();
		connection = client->connect_to_host(address, port);
		connected = true;

		if (reader.joinable())
			reader.join();
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

void TcpClient::read_data()
{
	const int max_size = 1024;
	char buffer[max_size];
	char *m_buff = buffer;
	auto input_stream = connection->get_input_stream();
	while (connected)
	{
		int size = gst_debugger_protocol_utils_read_header(input_stream->gobj(), cancel->gobj());

		if (size < 0)
			break;

		if (size > max_size)
		{
			m_buff = new char[size];
		}
		else
		{
			m_buff = buffer;
		}
		gst_debugger_protocol_utils_read_requested_size(input_stream->gobj(), size, m_buff, cancel->gobj());
		GstreamerInfo info;
		info.ParseFromArray(m_buff, size);
		if (m_buff != buffer)
		{
			delete m_buff;
		}
		signal_frame_received(info);
	}

	connected = false;
	signal_status_changed(false);
}

bool TcpClient::disconnect()
{
	connected = false;
	cancel->cancel();
	auto ok = connection->close();
	reader.join();
	return ok;
}

void TcpClient::write_data(char *data, int size)
{
	auto stream = connection->get_output_stream();
	stream->write(data, size);
}

void TcpClient::send_command(const Command &cmd)
{
	if (!is_connected())
		throw Gio::Error(Gio::Error::FAILED, "No connection!");

	char buffer[4];
	auto size = cmd.ByteSize();
	gst_debugger_protocol_utils_serialize_integer64(size, buffer, 4);
	connection->get_output_stream()->write(buffer, 4);
	cmd.SerializeToFileDescriptor(connection->get_socket()->get_fd());
}
