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
