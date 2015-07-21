/*
 * graph_module.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#include "graph_module.h"

#include "utils/gst-utils.h"

#include <graphviz-gstdebugger.h>

#include <boost/algorithm/string.hpp>

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
	graph_drawing_area->signal_motion_notify_event().connect(sigc::mem_fun(*this, &GraphModule::graphDrawingArea_motion_notify_cb));
	graph_drawing_area->signal_button_release_event().connect(sigc::mem_fun(*this, &GraphModule::graphDrawingArea_button_release_cb));
	set_drawing_area(GTK_WIDGET (graph_drawing_area->gobj()));

	builder->get_widget("selectedElementInGraphEntry", selected_element_entry);
	builder->get_widget("currentPathGraphEntry", current_path_graph_entry);

	builder->get_widget("upGraphButton", up_graph_button);
	up_graph_button->signal_clicked().connect(sigc::mem_fun(*this, &GraphModule::upGraphButton_clicked_cb));

	builder->get_widget("refreshGraphButton", refresh_graph_button);
	refresh_graph_button->signal_clicked().connect(sigc::mem_fun(*this, &GraphModule::refreshGraphButton_clicked_cb));

	builder->get_widget("elementPathPropertyEntry", element_path_property_entry);

	current_model = GraphElement::get_root();

	dsp.connect(sigc::mem_fun(*this, &GraphModule::redraw_model));
}

static std::vector<std::string> split_path(const std::string &path)
{
	std::vector<std::string> elements;
	boost::split(elements, path, [](char c) { return c == '/'; });

	elements.erase(std::remove_if(elements.begin(), elements.end(),
			[](const std::string &s){return s.empty();}), elements.end());
	return elements;
}

static void split_element_pad(const std::string &str, std::string &element, std::string &pad)
{
	auto pos = str.find(':');
	pad = str.substr(pos+1);
	element = str.substr(0, pos);
}

static std::shared_ptr<GraphElement> get_from_root(const std::vector<std::string>& elements)
{
	auto parent = GraphElement::get_root();

	for (std::size_t i = 0; i < elements.size(); i++)
	{
		auto it = std::find_if(parent->get_children().begin(), parent->get_children().end(),
				[&elements, i](std::shared_ptr<GraphElement> e) {return elements[i] == e->get_name();});

		if (it == parent->get_children().end())
			return std::shared_ptr<GraphElement>();

		parent = *it;
	}

	return parent;
}

void GraphModule::process_frame()
{
	if (info.info_type() == GstreamerInfo_InfoType_PROPERTY)
	{
		auto& p_info = info.property();
		GValue value = {0};
		g_value_init (&value, p_info.type());
		gst_value_deserialize(&value, p_info.property_value().c_str());

		std::shared_ptr<GValueBase> value_base(GValueBase::build_gvalue(&value));

		auto element = get_from_root(split_path (p_info.element_path()));

		if (element)
		{
			element->add_property(p_info.property_name(), value_base);
		}
	}

	if (info.info_type() != GstreamerInfo_InfoType_TOPOLOGY)
		return;

	if (info.topology().type() == Topology_ObjectType_ELEMENT)
	{
		auto& e_info = info.topology().element();
		auto elements = split_path (e_info.path());

		if (elements.empty())
			return;

		auto parent = get_from_root(std::vector<std::string>(elements.begin(), elements.end()-1));
		if (!parent)
			return;

		parent->add_child(std::make_shared<GraphElement>(elements.back(), e_info.type_name(), e_info.is_bin()));
	}
	else if (info.topology().type() == Topology_ObjectType_PAD)
	{
		auto& pad_tp = info.topology().pad();
		auto elements = split_path (pad_tp.path());

		if (elements.empty())
			return;

		std::string pad;
		split_element_pad(elements.back(), elements.back(), pad);

		auto parent = get_from_root(elements);
		if (!parent)
			return;

		parent->add_pad(std::make_shared<GraphPad>(pad, pad_tp.tpl_name(),
				pad_tp.is_ghostpad(), static_cast<Gst::PadDirection>(pad_tp.direction()),
				static_cast<Gst::PadPresence>(pad_tp.presence())));
	}
	else
	{
		auto& link = info.topology().link();
		auto src_elements = split_path (link.src_pad_path()),
				sink_elements = split_path (link.sink_pad_path());
		std::string src_pad, sink_pad;
		split_element_pad(src_elements.back(), src_elements.back(), src_pad);
		split_element_pad(sink_elements.back(), sink_elements.back(), sink_pad);
		auto src = get_from_root(src_elements)->get_pad(src_pad);
		auto sink = get_from_root(sink_elements)->get_pad(sink_pad);
		src->set_peer(sink);
	}

	dsp.emit();
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
				selected_element_entry->set_text(g_info->label->text);
				update_full_path();
			}
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

	return false;
}

void GraphModule::update_model(const std::shared_ptr<GraphElement>& new_model)
{
	current_model = new_model;

	std::shared_ptr<GraphObject> tmp_model = current_model;
	std::string model_path;
	while (tmp_model)
	{
		model_path = tmp_model->get_name() + "/" + model_path;
		tmp_model = tmp_model->get_parent();
	}
	current_path_graph_entry->set_text(model_path);

	redraw_model();
}

void GraphModule::upGraphButton_clicked_cb()
{
	auto new_model = std::dynamic_pointer_cast<GraphElement>(current_model->get_parent());
	if (new_model)
	{
		update_model(new_model);
	}
	else
	{
		// todo message: no parent
	}
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

	auto new_model = current_model->get_child(selected_element);

	if (!new_model || !new_model->is_bin())
	{
		// todo message: selected element is not a bin
		return;
	}

	update_model(new_model);
}

bool GraphModule::graphDrawingArea_motion_notify_cb(GdkEventMotion *event)
{
	GVJ_t *job;

	job = (GVJ_t *)g_object_get_data(G_OBJECT(graph_drawing_area->gobj()),"job");
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

void GraphModule::update_full_path()
{
	element_path_property_entry->set_text(
			current_path_graph_entry->get_text() + selected_element_entry->get_text());
}
