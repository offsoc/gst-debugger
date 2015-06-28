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
#include <gstreamermm.h>

class EventListModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	EventListModelColumns() {
		add(type); add(timestamp); add(seqnum); add(event);
	}

	Gtk::TreeModelColumn<gint32> type;
	Gtk::TreeModelColumn<guint64> timestamp;
	Gtk::TreeModelColumn<guint64> seqnum;
	Gtk::TreeModelColumn<GstEvent*> event;
};

class EventDetailsModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	EventDetailsModelColumns() {
		add(name); add(value);
	}

	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<Glib::ustring> value;
};

class EventTypesModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	EventTypesModelColumns()
	{add(type_name); add(type_id);}

	Gtk::TreeModelColumn<int> type_id;
	Gtk::TreeModelColumn<Glib::ustring> type_name;
};

class GstEventModule : public FrameReceiver
{
	std::shared_ptr<GstDebuggerTcpClient> client;

	Gtk::TreeView *event_list_tree_view;
	Gtk::TreeView *event_details_tree_view;
	Gtk::Button *start_watching_events_button;
	Gtk::ComboBox *event_types_combobox;
	Gtk::Entry *event_pad_path_entry;

	Glib::RefPtr<Gtk::ListStore> event_list_model;
	EventListModelColumns event_list_model_columns;

	Glib::RefPtr<Gtk::TreeStore> event_details_model;
	EventDetailsModelColumns event_details_model_columns;

	EventTypesModelColumns event_types_model_columns;
	Glib::RefPtr<Gtk::ListStore> event_types_model;

	void process_frame() override;

	void append_event_entry();
	void display_event_details(const Glib::RefPtr<Gst::Event>& event);

	void startWatchingEventsButton_click_cb();
	void eventListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);

public:
	GstEventModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client);
};

#endif /* SRC_GST_DEBUGGER_GST_EVENT_MODULE_H_ */
