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
	Glib::Value<Glib::RefPtr<Gst::Caps>> v;
	v.init(g_value);

	return v.get()->to_string();
}

Gtk::Widget* GValueCaps::get_widget() const
{
	Gtk::Entry *widget = Gtk::manage(new Gtk::Entry());
	widget->set_text(to_string());
	return widget;
}

