/*
 * graph_module.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#include "graph_module.h"

#include <graphviz-gstdebugger.h>

GraphModule::GraphModule(const Glib::RefPtr<Gst::Bin>& model, const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client)
: client (client),
  model(model)
{
	builder->get_widget("graphDrawingArea", graph_drawing_area);
	graph_drawing_area->signal_draw().connect(sigc::mem_fun(*this, &GraphModule::graphDrawingArea_draw_cb));
	graph_drawing_area->signal_button_press_event().connect(sigc::mem_fun(*this, &GraphModule::graphDrawingArea_button_press_event_cb));
	graph_drawing_area->set_events(Gdk::EXPOSURE_MASK |
			Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK |
			Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
			Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK);

	set_drawing_area(GTK_WIDGET (graph_drawing_area->gobj()));

	builder->get_widget("selectedElementInGraphEntry", selected_element_entry);
	builder->get_widget("currentPathGraphEntry", current_path_graph_entry);

	builder->get_widget("upGraphButton", up_graph_button);
	up_graph_button->signal_clicked().connect(sigc::mem_fun(*this, &GraphModule::upGraphButton_clicked_cb));

	builder->get_widget("jumpToGraphButton", jump_to_graph_button);
	jump_to_graph_button->signal_clicked().connect(sigc::mem_fun(*this, &GraphModule::jumpToGraphButton_clicked_cb));

	builder->get_widget("refreshGraphButton", refresh_graph_button);
	refresh_graph_button->signal_clicked().connect(sigc::mem_fun(*this, &GraphModule::refreshGraphButton_clicked_cb));
}

void GraphModule::process_frame()
{

}

bool GraphModule::graphDrawingArea_button_press_event_cb(GdkEventButton  *event)
{
	GVJ_t *job;
	pointf pointer;
	Agraphinfo_t *g_info;
	Agnodeinfo_t *n_info;
	Agedgeinfo_t *e_info;

	job = (GVJ_t *)g_object_get_data(G_OBJECT(graph_drawing_area->gobj()),"job");
	pointer.x = event->x;
	pointer.y = event->y;
	(job->callbacks->button_press)(job, event->button, pointer);
	if (job->selected_obj) {
		switch (agobjkind(job->selected_obj)) {
		case AGRAPH:
			g_info = (Agraphinfo_t*)(((Agobj_t*)(job->selected_obj))->data);
			if (g_info->label != NULL)
				selected_element_entry->set_text(g_info->label->text);
			break;
		case AGNODE:
			n_info = (Agnodeinfo_t*)(((Agobj_t*)(job->selected_obj))->data);
			if (n_info->label != NULL)
			{
				auto tmp = (Agraph_t*)n_info->clust;
				g_info = (Agraphinfo_t*)(((Agobj_t*)tmp)->data);
				if (g_info->label != NULL)
					selected_element_entry->set_text(g_info->label->text + std::string(":") + n_info->label->text);
			}
			break;
		case AGEDGE:
			break;
		}
	}

	return FALSE;
}

void GraphModule::update_model(const Glib::RefPtr<Gst::Bin>& new_model)
{
	model = new_model;

	auto tmp_model = model;
	std::string path;

	while (true)
	{
		auto bin = Glib::RefPtr<Gst::Bin>::cast_dynamic(tmp_model->get_parent());
		if (bin)
		{
			path = tmp_model->get_name() + "/" + path;
			tmp_model = bin;
		}
		else break;
	}
	current_path_graph_entry->set_text("/" + path);
	redraw_model();
}

void GraphModule::upGraphButton_clicked_cb()
{
	//auto p = model->get_u;
	auto new_model = Glib::RefPtr<Gst::Bin>::cast_dynamic(model->get_parent());
	if (new_model)
	{
		update_model(new_model);
	}
	else
	{
		// todo message: no parent
	}
}

void GraphModule::jumpToGraphButton_clicked_cb()
{
	std::string selected_element = selected_element_entry->get_text();

	if (selected_element.empty())
	{
		// todo message: no element selected
		return;
	}
	else if (selected_element.find(':') != std::string::npos)
	{
		// todo message: cannot jump to pad
		return;
	}

	auto e = model->get_element(selected_element);
	auto new_model = Glib::RefPtr<Gst::Bin>::cast_dynamic(model->get_element(selected_element));

	if (!new_model)
	{
		// todo message: selected element is not a bin
		return;
	}

	update_model(new_model);
}

bool GraphModule::graphDrawingArea_draw_cb(const Cairo::RefPtr<Cairo::Context>& context)
{
	GVJ_t *job = (GVJ_t *)g_object_get_data(G_OBJECT(graph_drawing_area->gobj()), "job");
	(job->callbacks->motion)(job, job->pointer);
	job->context = (void *)context->cobj();
	job->external_context = TRUE;
	job->width = graph_drawing_area->get_allocated_width();
	job->height = graph_drawing_area->get_allocated_height();
	(job->callbacks->refresh)(job);
	return false;
}

void GraphModule::free_graph()
{
	if (gvc != nullptr)
	{
		if (g != nullptr)
			gvFreeLayout (gvc, g);
		gvFreeContext (gvc);
	}
	if (g != nullptr)
	{
		agclose (g);
	}
}

void GraphModule::redraw_model()
{
	free_graph();

	gvc = gvContext ();
	g = agmemread (dot_converter.to_dot_data(model).c_str());
	gvLayout (gvc, g, "dot");
	graph_drawing_area->hide();
	gvRender (gvc, g, "gstdebugger", NULL);
}

void GraphModule::refreshGraphButton_clicked_cb()
{
	auto e = Gst::ElementFactory::create_element("decodebin");
	model->add (e);

	redraw_model();
}
