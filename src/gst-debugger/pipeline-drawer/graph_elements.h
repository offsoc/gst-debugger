/*
 * graph_elements.h
 *
 *  Created on: Jul 16, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_ELEMENTS_H_
#define SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_ELEMENTS_H_

#include <gstreamermm.h>

#include <memory>
#include <string>

class GraphObject : public std::enable_shared_from_this<GraphObject>
{
	std::string name;
	std::shared_ptr<GraphObject> parent;

public:
	virtual ~GraphObject() {}
	GraphObject(const std::string &name, const std::shared_ptr<GraphObject>& parent = std::shared_ptr<GraphObject>())
	 : name(name), parent(parent) {}

	std::string get_name() const { return name; }
	std::shared_ptr<GraphObject> get_parent() const { return parent; }
	void set_parent(const std::shared_ptr<GraphObject>& parent)
	{
		this->parent = parent;
	}
};

class GraphPad : public GraphObject
{
	std::string tpl_name;
	bool is_ghost_pad;
	Gst::PadDirection direction;
	Gst::PadPresence presence;
	std::shared_ptr<GraphPad> peer;

public:
	GraphPad(const std::string &name, const std::string &tpl_name, bool is_ghost_pad,
			Gst::PadDirection direction, Gst::PadPresence presence)
	 : GraphObject(name), tpl_name(tpl_name), is_ghost_pad(is_ghost_pad),
	   direction(direction), presence(presence)
	{}

	bool is_ghost() const { return is_ghost_pad; }
	Gst::PadDirection get_direction() const { return direction; }
	Gst::PadPresence get_presence() const { return presence; }
	void set_peer(const std::shared_ptr<GraphPad>& peer) { this->peer = peer; }
	std::shared_ptr<GraphPad> get_peer() const { return peer; }
};

class GraphElement : public GraphObject
{
public:
	std::string type_name;
	bool is_bin_;
	std::vector<std::shared_ptr<GraphElement>> children;
	std::vector<std::shared_ptr<GraphPad>> pads;

public:
	GraphElement(const std::string &name, const std::string &type_name, bool is_bin)
	 : GraphObject(name), type_name(type_name), is_bin_(is_bin)
	{}

	bool is_bin() const { return is_bin_; }

	void add_child(const std::shared_ptr<GraphElement> &child)
	{
		children.push_back(child);
		child->set_parent(shared_from_this());
	}

	void add_pad(const std::shared_ptr<GraphPad> &pad)
	{
		if (get_pad(pad->get_name()))
			return;
		pads.push_back(pad);
		pad->set_parent(shared_from_this());
	}

	static std::shared_ptr<GraphElement> get_root()
	{
		static std::shared_ptr<GraphElement> root = std::make_shared<GraphElement>("", "", true);

		return root;
	}

	std::shared_ptr<GraphPad> get_pad(const std::string &pad_name)
	{
		auto it = std::find_if(pads.begin(), pads.end(), [pad_name](const std::shared_ptr<GraphPad>& pad) {
			return pad->get_name() == pad_name;
		});

		return (it != pads.end()) ? *it : std::shared_ptr<GraphPad>();
	}

	std::shared_ptr<GraphElement> get_child(const std::string &child_name)
	{
		auto it = std::find_if(children.begin(), children.end(), [child_name](const std::shared_ptr<GraphElement>& element) {
			return element->get_name() == child_name;
		});

		return (it != children.end()) ? *it : std::shared_ptr<GraphElement>();
	}
};

#endif /* SRC_GST_DEBUGGER_PIPELINE_DRAWER_GRAPH_ELEMENTS_H_ */
