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

#include <gvc.h>
#include <gvcjob.h>

#include <gtkmm.h>
#include <gstreamermm.h>

class GraphModule : public FrameReceiver
{
protected:
	std::shared_ptr<GstDebuggerTcpClient> client;
	Glib::RefPtr<Gst::Bin> model;
	GstBinToDotConverter dot_converter;

	Agraph_t *g = nullptr;
	GVC_t * gvc = nullptr;


	Gtk::DrawingArea *graph_drawing_area;
	Gtk::Button *up_graph_button;
	Gtk::Button *jump_to_graph_button;
	Gtk::Entry *selected_element_entry;
	Gtk::Entry *current_path_graph_entry;
	Gtk::Button *refresh_graph_button;

	void process_frame() override;

	void update_model(const Glib::RefPtr<Gst::Bin>& new_model);

	void upGraphButton_clicked_cb();
	void jumpToGraphButton_clicked_cb();
	bool graphDrawingArea_draw_cb(const Cairo::RefPtr<Cairo::Context>& context);
	bool graphDrawingArea_button_press_event_cb(GdkEventButton* event);
	void refreshGraphButton_clicked_cb();

public:
	GraphModule(const Glib::RefPtr<Gst::Bin>& pipeline, const Glib::RefPtr<Gtk::Builder>& builder,
			const std::shared_ptr<GstDebuggerTcpClient>& client);

	void redraw_model();
	void free_graph();
};
#endif /* SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_MODULE_H_ */
