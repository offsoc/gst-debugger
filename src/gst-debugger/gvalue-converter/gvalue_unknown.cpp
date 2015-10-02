/*
 * gvalue_unknown.cpp
 *
 *  Created on: Oct 1, 2015
 *      Author: loganek
 */

#include "gvalue_unknown.h"
#include "common/gst-utils.h"
GValueUnknown::GValueUnknown(GValue *gobj)
: GValueBase(gobj)
{}

std::string GValueUnknown::to_string() const
{
	GstUnknownType *unknown_type = (GstUnknownType*)g_value_get_boxed(g_value);
	return "unsupported type: " + std::string(unknown_type->type_name);
}

Gtk::Widget* GValueUnknown::create_widget()
{
	auto lbl = new Gtk::Label();

	lbl->set_text(to_string());

	return lbl;
}

