/*
 * gvalue_string.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#include "gvalue_string.h"

#include <glibmm.h>

GValueString::GValueString(GValue *gobj)
: GValueBase(gobj)
{}

std::string GValueString::to_string() const
{
	Glib::Value<Glib::ustring> v;
	v.init(g_value);

	return v.get();
}

