/*
 * gst_pipeline_model.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_GST_PIPELINE_MODEL_H_
#define SRC_GST_DEBUGGER_GST_PIPELINE_MODEL_H_

#include "gvalue-converter/gvalue_base.h"

#include <gstreamermm.h>

#include <memory>
#include <map>
#include <string>

class ObjectModel : public std::enable_shared_from_this<ObjectModel>
{
	std::string name;
	std::shared_ptr<ObjectModel> parent;

public:
	virtual ~ObjectModel() {}

	ObjectModel(const std::string &name, const std::shared_ptr<ObjectModel>& parent = std::shared_ptr<ObjectModel>())
	 : name(name), parent(parent) {}

	std::string get_name() const { return name; }

	std::shared_ptr<ObjectModel> get_parent() const { return parent; }

	void set_parent(const std::shared_ptr<ObjectModel>& parent) { this->parent = parent; }
};

class PadModel : public ObjectModel
{
	std::string tpl_name;
	bool is_ghost_pad;
	Gst::PadDirection direction;
	Gst::PadPresence presence;
	std::shared_ptr<PadModel> peer;

public:
	PadModel(const std::string &name, const std::string &tpl_name, bool is_ghost_pad,
			Gst::PadDirection direction, Gst::PadPresence presence)
	 : ObjectModel(name), tpl_name(tpl_name), is_ghost_pad(is_ghost_pad),
	   direction(direction), presence(presence)
	{}

	bool is_ghost() const { return is_ghost_pad; }

	Gst::PadDirection get_direction() const { return direction; }

	Gst::PadPresence get_presence() const { return presence; }

	void set_peer(const std::shared_ptr<PadModel>& peer) { this->peer = peer; }

	std::shared_ptr<PadModel> get_peer() const { return peer; }
};

class ElementModel : public ObjectModel
{
	std::string type_name;
	bool is_bin_;
	std::vector<std::shared_ptr<ElementModel>> children;
	std::vector<std::shared_ptr<PadModel>> pads;
	std::map<std::string, std::shared_ptr<GValueBase>> properties;

public:

	ElementModel(const std::string &name, const std::string &type_name, bool is_bin)
	 : ObjectModel(name), type_name(type_name), is_bin_(is_bin)
	{}

	bool is_bin() const { return is_bin_; }

	void add_child(const std::shared_ptr<ElementModel> &child);

	void add_pad(const std::shared_ptr<PadModel> &pad);

	static std::shared_ptr<ElementModel> get_root();

	std::shared_ptr<PadModel> get_pad(const std::string &pad_name);

	std::shared_ptr<ElementModel> get_child(const std::string &child_name);

	const std::vector<std::shared_ptr<ElementModel>>& get_children() const { return children; }

	const std::vector<std::shared_ptr<PadModel>>& get_pads() const { return pads; }

	void add_property(const std::string &name, const std::shared_ptr<GValueBase>& gvalue);
};

typedef std::shared_ptr<ObjectModel> GraphObjectPtr;
typedef std::shared_ptr<PadModel> GraphPadPtr;
typedef std::shared_ptr<ElementModel> GraphElementPtr;

#endif /* SRC_GST_DEBUGGER_GST_PIPELINE_MODEL_H_ */
