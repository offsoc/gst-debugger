/*
 * topology_controller.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "topology_controller.h"
#include "element_path_processor.h"

#include <boost/algorithm/string/split.hpp>

void TopologyController::process(const Topology& topology)
{
	if (topology.type() == Topology_ObjectType_ELEMENT)
	{
		auto e_info = topology.element();
		ElementPathProcessor path_processor(e_info.path());
		auto parent = path_processor.get_parent_element();
		if (!parent)
			return;

		parent->add_child(std::make_shared<ElementModel>(path_processor.get_last_obj_str(), e_info.type_name(), e_info.is_bin()));
	}
	else if (topology.type() == Topology_ObjectType_PAD)
	{
		auto& pad_tp = topology.pad();
		ElementPathProcessor path_processor(pad_tp.path());
		auto parent = path_processor.get_parent_element();
		if (!parent)
			return;

		parent->add_pad(std::make_shared<PadModel>(path_processor.get_last_obj_str(), pad_tp.tpl_name(),
				pad_tp.is_ghostpad(), static_cast<Gst::PadDirection>(pad_tp.direction()),
				static_cast<Gst::PadPresence>(pad_tp.presence())));
	}
	else
	{
		auto& link = topology.link();
		ElementPathProcessor src_processor(link.src_pad_path()), sink_processor(link.sink_pad_path());
		std::shared_ptr<PadModel> src = std::dynamic_pointer_cast<PadModel>(src_processor.get_last_obj()),
				sink = std::dynamic_pointer_cast<PadModel>(sink_processor.get_last_obj());

		if (src == nullptr || sink == nullptr)
			return;

		src->set_peer(sink);
		if (sink->get_direction() == Gst::PAD_SINK)
		{
			sink->set_peer(src);
		}
	}
}
