#include "protocol/gstdebugger.pb.h"

#include <giomm.h>
#include <glibmm.h>

#include <cassert>
#include <thread>
#include <iostream>

static void read_requested_size(const Glib::RefPtr<Gio::InputStream>& input_stream, int requested_size, char *buffer)
{
	int size = 0;
	while (size < requested_size)
	{
		int cnt = input_stream->read(buffer + size, requested_size-size);
		size += cnt;
	}
}

static int read_header(const Glib::RefPtr<Gio::InputStream>& input_stream)
{
	char buffer[4];

	read_requested_size(input_stream, 4, buffer);
	int size = 0;
	for (int i = 0; i < 4; i++)
		size &= buffer[i] << i;

	return size;
}

static void read_data(const Glib::RefPtr<Gio::InputStream>& input_stream)
{
	char buffer[1024];

	int size = read_header(input_stream);
	assert(size <= 1024);
	read_requested_size(input_stream, size, buffer);

	GstreamerInfo info;

	info.ParseFromArray(buffer, size);

	std::cout << info.info_type() << std::endl;
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

	do {
		std::cin >> command;

		if (command == "start")
		{
			PadWatch *watch = new PadWatch();
			watch->set_pad_path("videotestsrc:src");
			watch->set_toggle(ENABLE);
			watch->set_watch_type(PadWatch_WatchType_BUFFER);

			Command cmd;
			cmd.set_command_type(Command_CommandType_PAD_WATCH);
			cmd.set_allocated_pad_watch(watch);
			cmd.SerializeToFileDescriptor(connection->get_socket()->get_fd());
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
