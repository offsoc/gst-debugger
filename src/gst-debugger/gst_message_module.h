/*
 * gst_message_module.h
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_MESSAGE_MODULE_H_
#define SRC_GST_DEBUGGER_GST_MESSAGE_MODULE_H_

#include "common_model_columns.h"
#include "controller/tcp_client.h"
#include "gst_qe_module.h"

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


// todo inherit from GstQEModule, a lot of copy&paste
class GstMessageModule : public GstQEModule
{
	void append_qe_entry() override;

	void display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe) override;

	void update_hook_list() override;

	void send_start_stop_command(bool enable) override;

public:
	GstMessageModule(const Glib::RefPtr<Gtk::Builder>& builder);
};


#endif /* SRC_GST_DEBUGGER_GST_MESSAGE_MODULE_H_ */
