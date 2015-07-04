#include "protocol/gstdebugger.pb.h"
#include "protocol/protocol_utils.h"

#include <giomm.h>
#include <glibmm.h>

#include <cassert>
#include <thread>
#include <iostream>


static void read_data(const Glib::RefPtr<Gio::InputStream>& input_stream)
{
	char buffer[1024];

	//while (true)
	{
		int size = gst_debugger_protocol_utils_read_header(input_stream->gobj());
		assert(size <= 1024);
		gst_debugger_protocol_utils_read_requested_size(input_stream->gobj(), size, buffer);

		GstreamerInfo info;

		info.ParseFromArray(buffer, size);

		std::cout << info.info_type() << std::endl;

		switch (info.info_type())
		{
		case GstreamerInfo_InfoType_LOG:
			std::cout << info.log().message() << std::endl;
			break;
		default:
			break;
		}
	}
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cerr << "usage: " << argv[0] << " <server ip> <port>" << std::endl;
		return -1;
	}

	Gio::init();

	auto client = Gio::SocketClient::create();
	auto connection = client->connect_to_host(argv[1], atoi(argv[2]));
	auto input_stream = connection->get_input_stream();

	std::thread th(read_data, input_stream);

	std::string command;
	char buffer[4];
	int size;
	do {
		std::cin >> command;

		if (command == "send")
		{
			PadWatch *pad_watch = new PadWatch();
			pad_watch->set_pad_path("videotestsrc:src");
			pad_watch->set_toggle(DISABLE);
			pad_watch->set_watch_type(PadWatch_WatchType_QUERY);

			Command cmd;
			cmd.set_command_type(Command_CommandType_PAD_WATCH);
			cmd.set_allocated_pad_watch(pad_watch);

			size = cmd.ByteSize();
			gst_debugger_protocol_utils_serialize_integer64(size, buffer, 4);
			connection->get_output_stream()->write(buffer, 4);
			cmd.SerializeToFileDescriptor(connection->get_socket()->get_fd());

			LogWatch *log_watch = new LogWatch();
			//log_watch->set_log_category("dummy");
			log_watch->set_log_level(10);

			Command cmd2;
			cmd2.set_command_type(Command_CommandType_LOG_WATCH);
			cmd2.set_allocated_log_watch(log_watch);

			size = cmd2.ByteSize();
			gst_debugger_protocol_utils_serialize_integer64(size, buffer, 4);
			connection->get_output_stream()->write(buffer, 4);
			cmd2.SerializeToFileDescriptor(connection->get_socket()->get_fd());
		}
		else if (command == "category")
		{
			std::string cat_str;
			std::cin >> cat_str;

			LogThreshold *log_threshold = new LogThreshold();
			log_threshold->set_list(cat_str);
			log_threshold->set_overwrite(1);

			Command cmd3;
			cmd3.set_command_type(Command_CommandType_LOG_THRESHOLD);
			cmd3.set_allocated_log_threshold(log_threshold);

			size = cmd3.ByteSize();
			gst_debugger_protocol_utils_serialize_integer64(size, buffer, 4);
			connection->get_output_stream()->write(buffer, 4);
			cmd3.SerializeToFileDescriptor(connection->get_socket()->get_fd());
		}
		else if (command == "message")
		{
			MessageWatch *msg_watch = new MessageWatch();
			msg_watch->set_message_type(16384);
			msg_watch->set_toggle(ENABLE);

			Command cmd4;
			cmd4.set_command_type(Command_CommandType_MESSAGE_WATCH);
			cmd4.set_allocated_message_watch(msg_watch);

			size = cmd4.ByteSize();
			gst_debugger_protocol_utils_serialize_integer64(size, buffer, 4);
			connection->get_output_stream()->write(buffer, 4);
			cmd4.SerializeToFileDescriptor(connection->get_socket()->get_fd());
		}
		else if (command == "log")
		{
			LogWatch *log_watch = new LogWatch();
			log_watch->set_log_level(0); // todo
			log_watch->set_toggle(ENABLE);

			Command cmd4;
			cmd4.set_command_type(Command_CommandType_LOG_WATCH);
			cmd4.set_allocated_log_watch(log_watch);

			size = cmd4.ByteSize();
			gst_debugger_protocol_utils_serialize_integer64(size, buffer, 4);
			connection->get_output_stream()->write(buffer, 4);
			cmd4.SerializeToFileDescriptor(connection->get_socket()->get_fd());
		}
	} while (command != "exit");

	try
	{
		input_stream->close();
	}
	catch (const Gio::Error& error)
	{
		std::cout << error.what() << std::endl;
	}

	th.join();

	return 0;
}
