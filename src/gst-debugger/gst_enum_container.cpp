/*
 * gst_enum_container.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "gst_enum_container.h"

void GstEnumContainer::process_frame()
{
	if (info.info_type() != GstreamerInfo_InfoType_ENUM_TYPE)
	{
		return;
	}

	auto& enum_info = info.enum_type();
	std::string type_name = enum_info.type_name();

	GstEnumType type(type_name);

	for (int i = 0; i < enum_info.entry_size(); i++)
	{
		type.add_value(enum_info.entry(i).name(), enum_info.entry(i).value());
	}

	auto it = get_enum_type_it(type_name);

	if (it != types.end())
	{
		*it = type;
	}
	else
	{
		types.push_back(type);
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
