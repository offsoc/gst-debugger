/*
 * gvalue_enum.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#include "gvalue_enum.h"

#include "common/gst-utils.h"

#include <glibmm.h>

GValueEnum::GValueEnum(GValue *gobj)
: GValueBase(gobj)
{}

gint GValueEnum::get_value() const
{
	return g_value_get_enum(g_value);
}

std::string GValueEnum::to_string() const
{
	gint value = get_value();
	if (!type)
	{
		return std::to_string(value) + " (press 'refresh' button)";
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

Gtk::Widget* GValueEnum::create_widget()
{
	if (type)
	{
		Gtk::ComboBoxText *widget = new Gtk::ComboBoxText();
		widget->signal_changed().connect([this, widget]{
			if (!emit_m.try_lock()) return;
			g_value_set_enum(g_value, gtk_combo_box_get_active(GTK_COMBO_BOX(widget->gobj())));
			widget_value_changed();
			emit_m.unlock();
		});

		for (auto entry : type.get().get_values())
		{
			widget->append(entry.second.nick);
		}

		return widget;
	}
	else
	{
		return new Gtk::Label();
	}
}

void GValueEnum::update_widget(Gtk::Widget* widget)
{
	if (dynamic_cast<Gtk::ComboBoxText*>(widget))
	{
		gint pos = 0;
		gint value = get_value();
		for (auto entry : type.get().get_values())
		{
			if (entry.first == value)
			{
				emit_m.lock();
				dynamic_cast<Gtk::ComboBoxText*>(widget)->set_active(pos);
				emit_m.unlock();
				break;
			}
			pos++;
		}
	}
	else
	{
		dynamic_cast<Gtk::Label*>(widget)->set_text(to_string());
	}
}
