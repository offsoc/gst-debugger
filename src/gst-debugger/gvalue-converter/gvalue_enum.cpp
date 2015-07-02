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

template<typename T>
void append_to_vector(std::vector<std::pair<int, std::string>>& values, T *enum_values)
{
	int i = 0;

	while (enum_values[i].value_name)
	{
		values.push_back(std::make_pair(enum_values[i].value, enum_values[i].value_nick));
		i++;
	}
}

std::string GValueEnum::to_string() const
{
	gpointer ptr = g_type_class_ref(G_VALUE_TYPE(g_value));
	if (G_IS_ENUM_CLASS(ptr))
	{
		Glib::Value_Enum<int> v;
		v.init(g_value);
		return G_ENUM_CLASS (ptr)->values[v.get()].value_nick;
	}
	else
	{
		Glib::Value_Flags<int> v;
		v.init(g_value);
		return G_FLAGS_CLASS (ptr)->values[v.get()].value_nick;
	}
}

std::vector<std::pair<int, std::string>> GValueEnum::get_values(GType type)
{
	std::vector<std::pair<int, std::string>> values;
	gpointer ptr = g_type_class_ref(type);

	if (G_IS_ENUM_CLASS (ptr))
	{
		append_to_vector(values, G_ENUM_CLASS (ptr)->values);
	}
	else
	{
		append_to_vector(values, G_FLAGS_CLASS (ptr)->values);
	}

	return values;
}
