/*
 * gst_message_module.h
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_MESSAGE_MODULE_H_
#define SRC_GST_DEBUGGER_GST_MESSAGE_MODULE_H_

#include "common_model_columns.h"
#include "gst_debugger_tcp_client.h"
#include "frame_receiver.h"

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
class GstMessageModule : public FrameReceiver
{
protected:
	std::shared_ptr<GstDebuggerTcpClient> client;

	Gtk::ComboBox *bus_message_types_combo_box;
	Gtk::Button *start_bus_message_button;
	Gtk::TreeView *existing_message_hooks_tree_view;
	Gtk::TreeView *message_list_tree_view;
	Gtk::TreeView *message_details_tree_view;

	TypesModelColumns message_types_model_columns;
	Glib::RefPtr<Gtk::ListStore> message_types_model;

	MsgHooksModelColumns message_hooks_model_columns;
	Glib::RefPtr<Gtk::ListStore> message_hooks_model;

	Glib::RefPtr<Gtk::ListStore> msg_list_model;
	ListModelColumns msg_list_model_columns;

	Glib::RefPtr<Gtk::TreeStore> msg_details_model;
	DetailsModelColumns msg_details_model_columns;

	void process_frame() override;
	void update_hook_list();
	void append_message_entry();
	void append_details_from_structure(Gst::Structure& structure);
	void append_details_row(const std::string &name, const std::string &value);

	void startBusMessageButton_clicked_cb();
	void MessageListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);

public:
	GstMessageModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client);
};


#endif /* SRC_GST_DEBUGGER_GST_MESSAGE_MODULE_H_ */
