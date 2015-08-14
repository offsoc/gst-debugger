/*
 * graph_module.h
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_MODULE_H_
#define SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_MODULE_H_

#include "controller/iview.h"
#include "gst_bin_to_dot_converter.h"
#include "../models/gst_pipeline_model.h"

#include <gvc.h>
#include <gvcjob.h>

#include <gtkmm.h>

class GraphModule : public IBaseView
{
protected:

	GstBinToDotConverter dot_converter;

	Agraph_t *g = nullptr;
	GVC_t * gvc = nullptr;

	Gtk::DrawingArea *graph_drawing_area;
	Gtk::Button *up_graph_button;
	Gtk::Entry *selected_element_entry;
	Gtk::Entry *current_path_graph_entry;
	Gtk::Button *refresh_graph_button;

	GMainLoop *graphviz_plugin_loop = NULL;

	void jump_to_selected_model();

	void upGraphButton_clicked_cb();
	bool graphDrawingArea_draw_cb(const Cairo::RefPtr<Cairo::Context>& context);
	bool graphDrawingArea_button_press_event_cb(GdkEventButton* event);
	bool graphDrawingArea_motion_notify_cb(GdkEventMotion *event);
	bool graphDrawingArea_button_release_cb(GdkEventButton *event);
	void refreshGraphButton_clicked_cb();

	void update_selected_object();
	void update_selected_object_();

	bool drawing_area_events_allowed() const;

public:
	GraphModule(const Glib::RefPtr<Gtk::Builder>& builder);
	~GraphModule();

	void update_model_();
	void update_model(std::shared_ptr<ElementModel> new_model);
	void free_graph();

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};
#endif /* SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_MODULE_H_ */
