/*
 * graph_module.h
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_MODULE_H_
#define SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_MODULE_H_

#include "frame_receiver.h"
#include "gst_debugger_tcp_client.h"
#include "gst_bin_to_dot_converter.h"

#include <gtkmm.h>
#include <gstreamermm.h>

class GraphModule : public FrameReceiver
{
protected:
	std::shared_ptr<GstDebuggerTcpClient> client;
	Glib::RefPtr<Gst::Bin> model;
	GstBinToDotConverter dot_converter;

	Gtk::DrawingArea *graph_drawing_area;
	Gtk::Button *up_graph_button;
	Gtk::Button *jump_to_graph_button;
	Gtk::Entry *selected_element_entry;

	void process_frame() override;

	void jumpToGraphButton_clicked_cb();
	bool graphDrawingArea_draw_cb(const Cairo::RefPtr<Cairo::Context>& context);
	bool graphDrawingArea_button_press_event_cb(GdkEventButton* event);

public:
	GraphModule(const Glib::RefPtr<Gst::Bin>& pipeline, const Glib::RefPtr<Gtk::Builder>& builder,
			const std::shared_ptr<GstDebuggerTcpClient>& client);

	void redraw_model();
};
#endif /* SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_MODULE_H_ */
