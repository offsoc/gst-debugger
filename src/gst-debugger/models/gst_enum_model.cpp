/*
 * gst_enum_container.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "gst_enum_model.h"

#include <algorithm>

#include <cassert>


void GstEnumContainer::update_type(const GstEnumType &type)
{
	auto it = get_enum_type_it(type.get_type_name());
	if (it == types.end())
	{
		types.push_back(type);
	}
	else
	{
		*it = type;
	}
}

std::vector<GstEnumType>::iterator GstEnumContainer::get_enum_type_it(const std::string &type_name)
{
	std::vector<GstEnumType>::iterator it = std::find_if(types.begin(), types.end(), [type_name] (const GstEnumType& t) {
		return t.get_type_name() == type_name;
	});

	return it;
}

bool GstEnumContainer::has_type(const std::string &type_name)
{
	return get_enum_type_it(type_name) != types.end();
}

GstEnumType GstEnumContainer::get_type(const std::string &type_name)
{
	auto type_it = get_enum_type_it(type_name);
	assert(type_it != types.end());
	return *type_it;
}
