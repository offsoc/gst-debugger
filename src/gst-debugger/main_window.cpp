/*
 * main_window.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "main_window.h"
#include "sigc++lambdahack.h"
MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Window(cobject),
  builder(builder),
  client(std::make_shared<GstDebuggerTcpClient>()),
  dispatcher(std::make_shared<Glib::Dispatcher>()),
  log_module(std::make_shared<GstLogModule>(builder, client)),
  event_module(std::make_shared<GstEventModule>(builder, client)),
  query_module(std::make_shared<GstQueryModule>(builder, client)),
  message_module(std::make_shared<GstMessageModule>(builder, client)),
  buffer_module(std::make_shared<GstBufferModule>(builder, client)),
  properties_module(std::make_shared<GstPropertiesModule>(builder, client)),
  enums(std::make_shared<GstEnumContainer>())
{
	graph_module = std::make_shared<GraphModule>(builder, client);

	builder->get_widget("connectionPropertiesMenuItem", connection_properties);
	connection_properties->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::connectionPropertiesMenuItem_activate_cb));

	builder->get_widget("connectMenuItem", connect_menu_item);
	connect_menu_item->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::connectMenuItem_activate_cb));

	builder->get_widget_derived("connectionPropertiesDialog", connection_properties_dialog);
	builder->get_widget("mainStatusbar", main_statusbar);

	client->signal_status_changed.connect(sigc::mem_fun(*this, &MainWindow::connection_status_changed));
	client->signal_frame_received.connect([this](const GstreamerInfo &info){
		for (auto receiver : data_receivers)
			receiver->on_frame_recieved(info);
		dispatcher->emit();
	});
	connection_status_changed(false);
	data_receivers.push_back(log_module);
	data_receivers.push_back(event_module);
	data_receivers.push_back(query_module);
	data_receivers.push_back(message_module);
	data_receivers.push_back(buffer_module);
	data_receivers.push_back(graph_module);
	data_receivers.push_back(properties_module);

	signal_show().connect([this] {
		graph_module->redraw_model();
	});

	signal_delete_event().connect([this](GdkEventAny*){
		graph_module->free_graph();
		return false;
	});
}

void MainWindow::connectionPropertiesMenuItem_activate_cb()
{
	connection_properties_dialog->show();
}

void MainWindow::connectMenuItem_activate_cb()
{
	if (client->is_connected())
		client->disconnect();
	else
		client->connect(
			connection_properties_dialog->get_ip_address(),
			connection_properties_dialog->get_port());
}

void MainWindow::connection_status_changed(bool connected)
{
	auto id = main_statusbar->get_context_id("connection-status");
	if (connected)
	{
		main_statusbar->push("Connected", id);
		((Gtk::Label*)connect_menu_item->get_child())->set_text("Disconnect");
	}
	else
	{
		main_statusbar->push("Disconnected", id);
		((Gtk::Label*)connect_menu_item->get_child())->set_text("Connect");
	}
}

