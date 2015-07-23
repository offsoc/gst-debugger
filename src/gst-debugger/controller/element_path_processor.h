/*
 * element_path_processor.h
 *
 *  Created on: Jul 23, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_ELEMENT_PATH_PROCESSOR_H_
#define SRC_GST_DEBUGGER_CONTROLLER_ELEMENT_PATH_PROCESSOR_H_

#include "models/gst_pipeline_model.h"

#include <boost/algorithm/string/split.hpp>

#include <vector>

class ElementPathProcessor
{
private:
	std::string last_obj;
	std::shared_ptr<ElementModel> parent_element;
	bool is_pad_ = false;
	std::vector<std::string> elements;

	void split(const std::string &path)
	{
		boost::split(elements, path, [](char c) { return c == '/'; });
		elements.erase(std::remove_if(elements.begin(), elements.end(),
				[](const std::string &s){return s.empty();}), elements.end());
	}

public:
	ElementPathProcessor (const std::string &path)
	{
		parent_element = ElementModel::get_root();
		split(path);

		if (elements.empty())
			return;

		auto pos = elements.back().find(':');
		is_pad_ = pos != std::string::npos;
		if (is_pad_)
		{
			last_obj = elements.back().substr(pos+1);
			elements.back() = elements.back().substr(0, pos);
		}
		else
		{
			last_obj = elements.back();
			elements.pop_back();
		}
		for (std::size_t i = 0; i < elements.size(); i++)
		{
			auto it = std::find_if(parent_element->get_children().begin(), parent_element->get_children().end(),
					[this, i](std::shared_ptr<ElementModel> e) {return elements[i] == e->get_name();});

			if (it == parent_element->get_children().end())
				return;

			parent_element = *it;
		}
	}

	std::shared_ptr<ElementModel> get_parent_element() const { return parent_element; }

	bool is_pad() const { return is_pad_; }

	std::string get_last_obj_str() const { return last_obj; }

	std::shared_ptr<ObjectModel> get_last_obj() const
	{
		if (is_pad_)
			return parent_element->get_pad(last_obj);
		else
			return parent_element->get_child(last_obj);
	}
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_ELEMENT_PATH_PROCESSOR_H_ */
