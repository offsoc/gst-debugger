/*
 * gst_properties_module.h
 *
 *  Created on: Jul 14, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_GST_PROPERTIES_MODULE_H_
#define SRC_GST_DEBUGGER_GST_PROPERTIES_MODULE_H_

#include "frame_receiver.h"
#include "gst_debugger_tcp_client.h"
#include "gvalue-converter/gvalue_base.h"

#include <gtkmm.h>

class GstPropertiesModule : public FrameReceiver
{
private:
	Gtk::Button *show_propetries_button;
	Gtk::Box *properties_box;
	Gtk::Entry *element_path_property_entry;

	void showPropertiesButton_clicked_cb();

	std::string previous_element_path;
	std::vector<Gtk::Box*> property_widgets;
	std::shared_ptr<GstDebuggerTcpClient> client;

	void process_frame() override;
	void append_property(const std::shared_ptr<GValueBase>& value_base);
	bool update_property(const std::shared_ptr<GValueBase>& value_base);
	void request_property(const std::string &property_name);

public:
	GstPropertiesModule(const Glib::RefPtr<Gtk::Builder>& builder,
			const std::shared_ptr<GstDebuggerTcpClient>& client);
};

#endif /* SRC_GST_DEBUGGER_GST_PROPERTIES_MODULE_H_ */
