/*
 * main_window.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_MAIN_WINDOW_H_
#define SRC_GST_DEBUGGER_MAIN_WINDOW_H_

#include "gst_debugger_tcp_client.h"
#include "connection_properties_dialog.h"
#include "gst_log_module.h"
#include "gst_event_module.h"
#include "gst_query_module.h"
#include "gst_message_module.h"
#include "gst_buffer_module.h"
#include "pipeline-drawer/graph_module.h"
#include "frame_receiver.h"

#include <gtkmm.h>

class MainWindow : public Gtk::Window
{
	void connectionPropertiesMenuItem_activate_cb();
	void connectMenuItem_activate_cb();
	void connection_status_changed(bool connected);

	Glib::RefPtr<Gtk::Builder> builder;
	std::shared_ptr<GstDebuggerTcpClient> client;
	Gtk::MenuItem *connection_properties;
	Gtk::ImageMenuItem *connect_menu_item;
	ConnectionPropertiesDialog *connection_properties_dialog;
	Gtk::Statusbar *main_statusbar;

	std::shared_ptr<Glib::Dispatcher> dispatcher;
	std::vector<std::shared_ptr<FrameReceiver>> data_receivers;
	std::shared_ptr<GstLogModule> log_module;
	std::shared_ptr<GstEventModule> event_module;
	std::shared_ptr<GstQueryModule> query_module;
	std::shared_ptr<GstMessageModule> message_module;
	std::shared_ptr<GstBufferModule> buffer_module;
	std::shared_ptr<GraphModule> graph_module;

	GstreamerInfo info;

public:
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~MainWindow() {}
};

#endif /* SRC_GST_DEBUGGER_MAIN_WINDOW_H_ */
