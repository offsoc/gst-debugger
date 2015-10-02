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

Gtk::Widget* GValueBoolean::create_widget()
{
	auto cb = new Gtk::CheckButton();

	cb->signal_toggled().connect([this, cb]{ g_value_set_boolean(g_value, cb->get_active()); });
	cb->signal_toggled().connect(widget_value_changed);
	update_widget(cb);

	return cb;
}

void GValueBoolean::update_widget(Gtk::Widget* widget)
{
	dynamic_cast<Gtk::CheckButton*>(widget)->set_active(get_value());
}
