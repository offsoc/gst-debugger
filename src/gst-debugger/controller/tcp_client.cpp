/*
 * gst_debugger_tcp_client.cpp
 *
 *  Created on: Jun 21, 2015
 *      Author: mkolny
 */

#include "tcp_client.h"

#include "../../common/protocol-utils.h"

#include <glibmm/i18n.h>

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

		GstDebugger::GStreamerData data;
		data.ParseFromArray(m_buff, size);
		if (m_buff != buffer)
		{
            delete [] m_buff;
		}
		signal_frame_received(data);
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

void TcpClient::send_command(const GstDebugger::Command &cmd)
{
	if (!is_connected())
		throw Gio::Error(Gio::Error::FAILED, _("Client isn't connected to a server!"));

    GError *err = gst_debugger_protocol_write_header(connection->get_output_stream()->gobj(), cmd.ByteSize());

    if (err)
    {
        // TODO: emt error
        g_print ("cannot send size of data: %s\n", err->message);
        g_error_free (err);
    }
    else
    {
        cmd.SerializeToFileDescriptor(connection->get_socket()->get_fd());
    }
}
