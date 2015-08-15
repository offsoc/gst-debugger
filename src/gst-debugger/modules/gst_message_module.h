/*
 * gst_message_module.h
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_MESSAGE_MODULE_H_
#define SRC_GST_DEBUGGER_GST_MESSAGE_MODULE_H_

#include "common_model_columns.h"
#include "gst_qe_module.h"
#include "controller/controller.h"

#include <gtkmm.h>
#include <gstreamermm.h>

class MsgHooksModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	MsgHooksModelColumns() {
		add(type);
	}

	Gtk::TreeModelColumn<gint> type;
};

class GstMessageModule : public GstQEModule
{
	void append_qe_entry(GstreamerQEBM *qebm) override;

	void display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe, const std::string &pad_path) override;

	void confirmation_received_() override;

	void message_confirmation_received(const MessageWatch& watch);

	void send_start_stop_command(bool enable) override;

public:
	GstMessageModule(const Glib::RefPtr<Gtk::Builder>& builder);

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};


#endif /* SRC_GST_DEBUGGER_GST_MESSAGE_MODULE_H_ */
