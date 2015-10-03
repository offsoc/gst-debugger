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

Gtk::Widget* GValueString::create_widget()
{
	auto entry = new Gtk::Entry();
	entry->signal_activate().connect([this, entry] {
		g_value_set_string(g_value, entry->get_text().c_str());
		update_gvalue(this->g_value);
	});

	entry->signal_activate().connect(widget_value_changed);

	return entry;
}

void GValueString::update_widget(Gtk::Widget* widget)
{
	dynamic_cast<Gtk::Entry*>(widget)->set_text(to_string());
}
