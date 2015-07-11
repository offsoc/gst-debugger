/*
 * graph_module.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#include "graph_module.h"

#include "utils/gst-utils.h"

#include <graphviz-gstdebugger.h>

GraphModule::GraphModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client)
: client (client)
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

	root_model = current_model = Gst::Pipeline::create ();

	dsp.connect(sigc::mem_fun(*this, &GraphModule::redraw_model));
}

void GraphModule::process_frame()
{
	if (info.info_type() != GstreamerInfo_InfoType_TOPOLOGY)
		return;

	Glib::RefPtr<Gst::Element> e = Glib::wrap(gst_utils_get_element_from_path(GST_ELEMENT (root_model->gobj()), info.topology().bin_path().c_str()), true);
	Glib::RefPtr<Gst::Bin> bin = bin.cast_static(e);

	if (info.topology().type() == Topology_ObjectType_ELEMENT)
	{
		if (bin->get_element(info.topology().element().name()))
			return;

		bin->add (Gst::ElementFactory::create_element(info.topology().element().factory(), info.topology().element().name()));
	}
	else
	{
		Glib::RefPtr<Gst::Element> element = bin->get_element(info.topology().pad().element());
		std::string pad_name = info.topology().pad().name();
		std::string tpl_name = info.topology().pad().tpl_name();

		if (element->get_static_pad (pad_name)) {
			return;
		}

		Glib::RefPtr<Gst::Pad> pad = info.topology().pad().is_ghostpad() ?
				pad.cast_static(Gst::GhostPad::create(element->get_pad_template(tpl_name), pad_name)) :
			Gst::Pad::create(element->get_pad_template(tpl_name), pad_name);
		element->add_pad(pad);
	}

	dsp.emit();
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
	current_model = new_model;

	auto tmp_model = current_model;
	gchar *path = gst_utils_get_object_path (GST_OBJECT (current_model->gobj()));
	current_path_graph_entry->set_text(path);
	g_free (path);

	redraw_model();
}

void GraphModule::upGraphButton_clicked_cb()
{
	auto new_model = Glib::RefPtr<Gst::Bin>::cast_dynamic(current_model->get_parent());
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

	auto e = current_model->get_element(selected_element);
	auto new_model = Glib::RefPtr<Gst::Bin>::cast_dynamic(current_model->get_element(selected_element));

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
		gvFinalize (gvc);
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
	g = agmemread (dot_converter.to_dot_data(current_model).c_str());
	gvLayout (gvc, g, "dot");
	graph_drawing_area->hide();
	gvRender (gvc, g, "gstdebugger", NULL);
}

void GraphModule::refreshGraphButton_clicked_cb()
{
	Command cmd;
	cmd.set_command_type(Command_CommandType_TOPOLOGY);
	client->send_command(cmd);
}
