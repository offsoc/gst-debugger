/*
 * topology_controller.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "topology_controller.h"

#include <boost/algorithm/string/split.hpp>

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

static std::shared_ptr<ElementModel> get_from_root(const std::vector<std::string>& elements)
{
	auto parent = ElementModel::get_root();

	for (std::size_t i = 0; i < elements.size(); i++)
	{
		auto it = std::find_if(parent->get_children().begin(), parent->get_children().end(),
				[&elements, i](std::shared_ptr<ElementModel> e) {return elements[i] == e->get_name();});

		if (it == parent->get_children().end())
			return std::shared_ptr<ElementModel>();

		parent = *it;
	}

	return parent;
}


void TopologyController::process(const Topology& topology)
{
	if (topology.type() == Topology_ObjectType_ELEMENT)
	{
		auto& e_info = topology.element();
		auto elements = split_path (e_info.path());

		if (elements.empty())
			return;

		auto parent = get_from_root(std::vector<std::string>(elements.begin(), elements.end()-1));
		if (!parent)
			return;

		parent->add_child(std::make_shared<ElementModel>(elements.back(), e_info.type_name(), e_info.is_bin()));
	}
	else if (topology.type() == Topology_ObjectType_PAD)
	{
		auto& pad_tp = topology.pad();
		auto elements = split_path (pad_tp.path());

		if (elements.empty())
			return;

		std::string pad;
		split_element_pad(elements.back(), elements.back(), pad);

		auto parent = get_from_root(elements);
		if (!parent)
			return;

		parent->add_pad(std::make_shared<PadModel>(pad, pad_tp.tpl_name(),
				pad_tp.is_ghostpad(), static_cast<Gst::PadDirection>(pad_tp.direction()),
				static_cast<Gst::PadPresence>(pad_tp.presence())));
	}
	else
	{
		auto& link = topology.link();
		auto src_elements = split_path (link.src_pad_path()),
				sink_elements = split_path (link.sink_pad_path());
		std::string src_pad, sink_pad;
		split_element_pad(src_elements.back(), src_elements.back(), src_pad);
		split_element_pad(sink_elements.back(), sink_elements.back(), sink_pad);
		auto src = get_from_root(src_elements)->get_pad(src_pad);
		auto sink = get_from_root(sink_elements)->get_pad(sink_pad);
		src->set_peer(sink);
	}
}
