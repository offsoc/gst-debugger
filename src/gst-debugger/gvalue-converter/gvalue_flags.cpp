/*
 * gvalue_flags.cpp
 *
 *  Created on: Oct 2, 2015
 *      Author: loganek
 */

#include "gvalue_flags.h"
#include "ui_utils.h"

#include "../../common/gst-utils.h"

#include <glibmm/i18n.h>
#include <glibmm.h>

GValueFlags::GValueFlags(GValue *gobj)
: GValueBase(gobj)
{}

gint GValueFlags::get_value() const
{
	return g_value_get_flags(g_value);
}

std::string GValueFlags::to_string() const
{
	gint value = get_value();
	if (!type)
	{
		return flags_value_to_string(value) + _(" (press 'refresh' button)"); // todo move somewhere else
	}
	try
	{
		return type.get().get_values().at(value).nick;
	}
	catch (const std::out_of_range&)
	{
		return std::to_string(value);
	}
}

Gtk::Widget* GValueFlags::create_widget()
{
	if (type)
	{
		auto widget = new Gtk::ScrolledWindow();
		Gtk::Viewport *vp = Gtk::manage(new Gtk::Viewport(Gtk::Adjustment::create(30, 0, 20), Gtk::Adjustment::create(10, 0, 50)));
		Gtk::Box *box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));
		for (auto val : type->get_values())
		{
			if (val.first == -1) continue;
			auto cb = Gtk::manage(new Gtk::CheckButton(val.second.nick));
			cb->signal_clicked().connect([this, cb]{
				if (!emit_m.try_lock()) return;
				if (cb->get_active())
					g_value_set_flags(g_value, get_value() | GPOINTER_TO_INT(cb->get_data("flags-data-value")));
				else
					g_value_set_flags(g_value, get_value() & ~GPOINTER_TO_INT(cb->get_data("flags-data-value")));
				widget_value_changed();
				emit_m.unlock();
			});
			cb->set_data("flags-data-value", GINT_TO_POINTER(val.first));
			box->pack_start(*cb, false, 5);
		}

		vp->add(*box);
		widget->add(*vp);
		widget->show_all();

		return widget;
	}
	else
	{
		return new Gtk::Label();
	}
}

Gtk::Box* GValueFlags::get_internal_box(Gtk::Widget *widget)
{
	auto vp = dynamic_cast<Gtk::ScrolledWindow*>(widget)->get_child();
	if (!vp) return nullptr;
	auto bx = dynamic_cast<Gtk::Viewport*>(vp)->get_child();
	return dynamic_cast<Gtk::Box*>(bx);
}

void GValueFlags::update_widget(Gtk::Widget* widget)
{
	if (dynamic_cast<Gtk::ScrolledWindow*>(widget))
	{
		auto box = get_internal_box(widget);
		if (!box) return;
		gint value = get_value();
		for (auto w : box->get_children())
		{
			auto cb = dynamic_cast<Gtk::CheckButton*>(w);
			if (!cb) continue;
			int val = GPOINTER_TO_INT(cb->get_data("flags-data-value"));
			emit_m.lock();
			cb->set_active(val & value);
			emit_m.unlock();
		}
	}
	else
	{
		dynamic_cast<Gtk::Label*>(widget)->set_text(to_string());
	}
}

void GValueFlags::set_sensitive(bool sensitive)
{
	for (auto widget : widgets)
	{
		if (dynamic_cast<Gtk::ScrolledWindow*>(widget))
		{
			auto box = get_internal_box(widget);
			if (!box) continue;
			for (auto w : box->get_children())
				w->set_sensitive(sensitive);
		}
		else
			dynamic_cast<Gtk::Label*>(widget)->set_sensitive(sensitive);
	}
}
