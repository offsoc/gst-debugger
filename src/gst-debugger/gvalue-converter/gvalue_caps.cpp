/*
 * gvalue_caps.cpp
 *
 *  Created on: Aug 3, 2015
 *      Author: loganek
 */

#include "gvalue_caps.h"

#include <gstreamermm.h>
#include <glibmm.h>

GValueCaps::GValueCaps(GValue *gobj)
: GValueBase(gobj)
{}

std::string GValueCaps::to_string() const
{
	gchar *str = gst_caps_to_string(gst_value_get_caps(g_value));
	std::string caps_str = str;
	g_free(str);
	return caps_str;
}

Gtk::Widget* GValueCaps::create_widget()
{
	auto entry = new Gtk::Entry();
	entry->signal_activate().connect([this, entry] {
		GstCaps *caps = gst_caps_from_string(entry->get_text().c_str());
		gst_value_set_caps(this->g_value, caps);
		gst_caps_unref(caps);
		update_gvalue(this->g_value);
	});

	entry->signal_activate().connect(widget_value_changed);

	return entry;
}

void GValueCaps::update_widget(Gtk::Widget* widget)
{
	dynamic_cast<Gtk::Entry*>(widget)->set_text(to_string());
}
