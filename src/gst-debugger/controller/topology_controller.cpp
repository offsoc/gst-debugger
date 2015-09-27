/*
 * topology_controller.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "topology_controller.h"
#include "element_path_processor.h"

#include "ui_utils.h"

#include <boost/algorithm/string/split.hpp>

void TopologyController::process(const GstDebugger::TopologyInfo& topology)
{
	lock_topology();

	switch (topology.topology_type_case())
	{
	case GstDebugger::TopologyInfo::kElement:
		process_element(topology.element(), topology.action());
		break;
	case GstDebugger::TopologyInfo::kPad:
		process_pad(topology.pad(), topology.action());
		break;
	case GstDebugger::TopologyInfo::kLink:
		process_link(topology.link(), topology.action());
		break;
	default:
		break;
	}

	unlock_topology();
}

void TopologyController::process_element(const GstDebugger::TopologyElement &element, GstDebugger::Action action)
{
	ElementPathProcessor path_processor(element.path());
	auto parent = path_processor.get_parent_element();
	if (!parent)
		return;

	switch (action)
	{
	case GstDebugger::ADD:
		parent->add_child(std::make_shared<ElementModel>(path_processor.get_last_obj_str(), element.type_name(), element.is_bin()));
		break;
	case GstDebugger::REMOVE:
		parent->remove_child(path_processor.get_last_obj_str());
		break;
	default:
		break;
	}
}

void TopologyController::process_pad(const GstDebugger::TopologyPad &pad, GstDebugger::Action action)
{
	ElementPathProcessor path_processor(pad.path());
	auto parent = path_processor.get_parent_element();
	if (!parent)
		return;

	switch (action)
	{
	case GstDebugger::ADD:
		parent->add_pad(std::make_shared<PadModel>(path_processor.get_last_obj_str(),
				protocol_template_to_gst_template(pad.template_()),
				pad.is_ghostpad(), static_cast<Gst::PadDirection>(pad.direction()),
				static_cast<Gst::PadPresence>(pad.presence())));
		break;
	case GstDebugger::REMOVE:
		parent->remove_pad(path_processor.get_last_obj_str());
		break;
	}
}

void TopologyController::process_link(const GstDebugger::TopologyLink &link, GstDebugger::Action action)
{
	ElementPathProcessor src_processor(link.src_pad()), sink_processor(link.sink_pad());
	std::shared_ptr<PadModel> src = std::dynamic_pointer_cast<PadModel>(src_processor.get_last_obj()),
			sink = std::dynamic_pointer_cast<PadModel>(sink_processor.get_last_obj());

	switch (action)
	{
	case GstDebugger::ADD:
		if (src == nullptr || sink == nullptr)
			return;
		src->set_peer(sink);
		if (sink->get_direction() == Gst::PAD_SINK && !sink->is_ghost())
		{
			sink->set_peer(src);
		}
		break;
	case GstDebugger::REMOVE:
	{
		std::shared_ptr<PadModel> empty;
		if (src != nullptr)
			src->set_peer(empty);
		if (sink != nullptr)
			sink->set_peer(empty);
		break;
	}
	}
}
