/*
 * gst_model.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

/*
 * gst_pipeline_model.h
 *
 *  Created on: Jul 16, 2015
 *      Author: loganek
 */

#include "gst_pipeline_model.h"
#include "controller/element_path_processor.h"

void ElementModel::add_child(const std::shared_ptr<ElementModel> &child)
{
	children.push_back(child);
	child->set_parent(shared_from_this());
}

void ElementModel::remove_child(const std::string &child_name)
{
	auto it = std::find_if(children.begin(), children.end(), [child_name](const std::shared_ptr<ElementModel> &m){
		return m->get_name() == child_name;
	});

	if (it != children.end()) // == myVector.end() means the element was not found
		children.erase(it);
}

void ElementModel::add_pad(const std::shared_ptr<PadModel> &pad)
{
	if (get_pad(pad->get_name()))
		return;
	pads.push_back(pad);
	pad->set_parent(shared_from_this());
}


void ElementModel::remove_pad(const std::string &pad_name)
{
	auto it = find_pad(pad_name);
	if (it != pads.end())
		pads.erase(it);
}

std::shared_ptr<ElementModel> ElementModel::get_root()
{
	static std::shared_ptr<ElementModel> root = std::make_shared<ElementModel>("", "", true);

	return root;
}

std::shared_ptr<PadModel> ElementModel::get_pad(const std::string &pad_name)
{
	auto it = find_pad(pad_name);

	return (it != pads.end()) ? *it : std::shared_ptr<PadModel>();
}

std::shared_ptr<ElementModel> ElementModel::get_child(const std::string &child_name)
{
	auto it = std::find_if(children.begin(), children.end(), [child_name](const std::shared_ptr<ElementModel>& element) {
		return element->get_name() == child_name;
	});

	return (it != children.end()) ? *it : std::shared_ptr<ElementModel>();
}

/*void ElementModel::add_property(const std::string &name, const std::shared_ptr<GValueBase>& gvalue)
{
	if (properties.find(name) == properties.end())
	{
		properties[name] = gvalue;
	}
	else
	{
		gvalue->update_gvalue(gvalue);
	}
}
*/
std::shared_ptr<ElementModel> ElementModel::get_parent_element_from_path(const std::string &path)
{
	ElementPathProcessor proc(path);
	return std::dynamic_pointer_cast<ElementModel>(proc.get_last_obj());
}
