/*
 * gst_event_module.h
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_EVENT_MODULE_H_
#define SRC_GST_DEBUGGER_GST_EVENT_MODULE_H_

#include "gst_debugger_tcp_client.h"
#include "frame_receiver.h"

#include <gtkmm.h>

class EventListModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	EventListModelColumns() {
		add(type); add(timestamp); add(seqnum);\
	}

	Gtk::TreeModelColumn<gint32> type;
	Gtk::TreeModelColumn<guint64> timestamp;
	Gtk::TreeModelColumn<guint64> seqnum;
};

class GstEventModule : public FrameReceiver
{
	std::shared_ptr<GstDebuggerTcpClient> client;

	Gtk::TreeView *event_list_tree_view;
	Gtk::Button *start_watching_events_button;

	Glib::RefPtr<Gtk::ListStore> event_list_model;
	EventListModelColumns event_list_model_columns;

	void process_frame() override;

	void append_event_entry();

	void startWatchingEventsButton_click_cb();

public:
	GstEventModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client);
};

#endif /* SRC_GST_DEBUGGER_GST_EVENT_MODULE_H_ */
