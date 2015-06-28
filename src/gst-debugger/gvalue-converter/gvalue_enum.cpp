/*
 * gvalue_enum.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#include "gvalue_enum.h"

#include <glibmm.h>

GValueEnum::GValueEnum(GValue *gobj)
: GValueBase(gobj)
{}

std::string GValueEnum::to_string() const
{
	Glib::Value_Enum<int> v;
	v.init(g_value);
	GEnumValue *values = G_ENUM_CLASS (g_type_class_ref(G_VALUE_TYPE(g_value)))->values;

	return values[v.get()].value_nick;
}

std::vector<std::pair<int, std::string>> GValueEnum::get_values(GType type)
{
	std::vector<std::pair<int, std::string>> values;
	GEnumValue *enum_values = G_ENUM_CLASS (g_type_class_ref(type))->values;
	int i = 0;

	while (enum_values[i].value_name)
	{
		values.push_back(std::make_pair(enum_values[i].value, enum_values[i].value_nick));
		i++;
	}

	return values;
}
