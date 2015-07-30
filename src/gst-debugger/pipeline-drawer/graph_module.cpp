/*
 * graph_module.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#include "graph_module.h"

#include "controller/controller.h"

#include "utils/gst-utils.h"

#include <graphviz-gstdebugger.h>

#include <boost/algorithm/string.hpp>

static void ptr_free(std::shared_ptr<ElementModel>* ptr)
{
	delete ptr;
}

GraphModule::GraphModule(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("graphDrawingArea", graph_drawing_area);
	graph_drawing_area->signal_draw().connect(sigc::mem_fun(*this, &GraphModule::graphDrawingArea_draw_cb));
	graph_drawing_area->signal_button_press_event().connect(sigc::mem_fun(*this, &GraphModule::graphDrawingArea_button_press_event_cb));
	graph_drawing_area->set_events(Gdk::EXPOSURE_MASK |
			Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK |
			Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
			Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK);
	graph_drawing_area->signal_motion_notify_event().connect(sigc::mem_fun(*this, &GraphModule::graphDrawingArea_motion_notify_cb));
	graph_drawing_area->signal_button_release_event().connect(sigc::mem_fun(*this, &GraphModule::graphDrawingArea_button_release_cb));
	set_drawing_area(GTK_WIDGET (graph_drawing_area->gobj()));

	builder->get_widget("selectedElementInGraphEntry", selected_element_entry);
	builder->get_widget("currentPathGraphEntry", current_path_graph_entry);

	builder->get_widget("upGraphButton", up_graph_button);
	up_graph_button->signal_clicked().connect(sigc::mem_fun(*this, &GraphModule::upGraphButton_clicked_cb));

	builder->get_widget("refreshGraphButton", refresh_graph_button);
	refresh_graph_button->signal_clicked().connect(sigc::mem_fun(*this, &GraphModule::refreshGraphButton_clicked_cb));

	create_dispatcher("update-model", sigc::mem_fun(*this, &GraphModule::update_model_), (GDestroyNotify)ptr_free);
	create_dispatcher("update-selected-object", sigc::mem_fun(*this, &GraphModule::update_selected_object_), NULL);
}

void GraphModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_model_changed.connect(sigc::mem_fun(*this, &GraphModule::update_model));
	controller->on_selected_object_changed.connect(sigc::mem_fun(*this, &GraphModule::update_selected_object));
}

bool GraphModule::graphDrawingArea_button_press_event_cb(GdkEventButton  *event)
{
	if (event->type == GDK_2BUTTON_PRESS)
	{
		jump_to_selected_model();
		return false;
	}

	GVJ_t *job;
	pointf pointer;
	Agraphinfo_t *g_info;
	Agnodeinfo_t *n_info;
	Agedgeinfo_t *e_info;

	job = (GVJ_t *)g_object_get_data(G_OBJECT(graph_drawing_area->gobj()),"job");
	pointer.x = event->x;
	pointer.y = event->y;

	auto prev_obj = job->selected_obj;
	(job->callbacks->button_press)(job, event->button, pointer);

	if (prev_obj)
	{
		switch (agobjkind(prev_obj))
		{
		case AGRAPH:
			GD_gui_state((graph_t*)prev_obj) = 0;
			break;
		case AGNODE:
			ND_gui_state((node_t*)prev_obj) = 0;
			break;
		default:
			break;
		}
	}

	if (job->selected_obj)
	{
		switch (agobjkind(job->selected_obj)) {
		case AGRAPH:
			g_info = (Agraphinfo_t*)(((Agobj_t*)(job->selected_obj))->data);
			if (g_info->label != NULL)
			{
				controller->set_selected_object(g_info->label->text);
			}
			break;
		case AGNODE:
			n_info = (Agnodeinfo_t*)(((Agobj_t*)(job->selected_obj))->data);
			if (n_info->label != NULL)
			{
				auto tmp = (Agraph_t*)n_info->clust;
				g_info = (Agraphinfo_t*)(((Agobj_t*)tmp)->data);
				if (g_info->label != NULL)
				{
					controller->set_selected_object(g_info->label->text + std::string(":") + n_info->label->text);
				}
			}
			break;
		case AGEDGE:
			break;
		}
	}

	return false;
}

void GraphModule::update_model(std::shared_ptr<ElementModel> new_model)
{
	gui_push("update-model", new std::shared_ptr<ElementModel>(new_model));
	gui_emit("update-model");
}

void GraphModule::update_selected_object()
{
	gui_emit("update-selected-object");
}

void GraphModule::update_selected_object_()
{
	auto obj = controller->get_selected_object();

	if (obj)
	{
		std::string name = obj->get_name();
		if (std::dynamic_pointer_cast<PadModel>(obj))
		{
			name = obj->get_parent()->get_name() + ":" + name;
		}
		selected_element_entry->set_text(name);
	}
}

void GraphModule::upGraphButton_clicked_cb()
{
	controller->model_up();
}

void GraphModule::jump_to_selected_model()
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

	controller->model_down(selected_element);
}

bool GraphModule::graphDrawingArea_motion_notify_cb(GdkEventMotion *event)
{
	GVJ_t *job;
	job = (GVJ_t *)g_object_get_data(G_OBJECT(graph_drawing_area->gobj()),"job");

	if (job == nullptr)
		return false;

	job->pointer.x = event->x;
	job->pointer.y = event->y;
	graph_drawing_area->queue_draw();

	return false;
}

bool GraphModule::graphDrawingArea_button_release_cb(GdkEventButton *event)
{
	GVJ_t *job;
	pointf pointer;

	job = (GVJ_t *)g_object_get_data(G_OBJECT(graph_drawing_area->gobj()),"job");
	pointer.x = event->x;
	pointer.y = event->y;
	(job->callbacks->button_release)(job, event->button, pointer);

	return false;
}

bool GraphModule::graphDrawingArea_draw_cb(const Cairo::RefPtr<Cairo::Context>& context)
{
	GVJ_t *job = (GVJ_t *)g_object_get_data(G_OBJECT(graph_drawing_area->gobj()), "job");

	if (job == nullptr)
		return false;

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
		gvFreeContext (gvc);
		gvFinalize (gvc);
		gvc = nullptr;
	}
	if (g != nullptr)
	{
		agclose (g);
		g = nullptr;
	}
}

void GraphModule::update_model_()
{
	free_graph();

	auto m = gui_try_pop<std::shared_ptr<ElementModel>*>("update-model");
	std::string model_str;
	if (m != nullptr)
	{
		auto new_model = *m;

		std::shared_ptr<ObjectModel> tmp_model = new_model;
		model_str = dot_converter.to_dot_data(new_model);

		std::string model_path;
		while (tmp_model)
		{
			model_path = tmp_model->get_name() + "/" + model_path;
			tmp_model = tmp_model->get_parent();
		}
		current_path_graph_entry->set_text(model_path);
		delete m;
	}
	else
	{
		model_str = dot_converter.get_blank_page();
	}

	gvc = gvContext ();
	g = agmemread (model_str.c_str());
	gvLayout (gvc, g, "dot");
	graph_drawing_area->hide();
	gvRender (gvc, g, "gstdebugger", NULL);
}

void GraphModule::refreshGraphButton_clicked_cb()
{
	controller->send_request_topology_command();
}
