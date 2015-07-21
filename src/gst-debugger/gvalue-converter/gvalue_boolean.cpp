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

bool GValueBoolean::get_value() const
{
	Glib::Value<bool> v;
	v.init(g_value);
	return v.get();
}

std::string GValueBoolean::to_string() const
{
	Glib::Value<bool> v;
	v.init(g_value);

	return get_value() ? "true" : "false";
}

Gtk::Widget* GValueBoolean::get_widget() const
{
	Gtk::CheckButton *widget = Gtk::manage(new Gtk::CheckButton());
	widget->set_active(get_value());
	return widget;
}
