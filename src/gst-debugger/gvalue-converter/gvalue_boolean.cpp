/*
 * gvalue_boolean.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#include "gvalue_boolean.h"

#include <glibmm.h>

GValueBoolean::GValueBoolean(GValue *gobj)
: GValueBase(gobj)
{}

std::string GValueBoolean::to_string() const
{
	Glib::Value<bool> v;
	v.init(g_value);

	return v.get() ? "true" : "false";
}
