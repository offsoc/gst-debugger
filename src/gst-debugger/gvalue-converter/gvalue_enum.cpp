/*
 * gvalue_enum.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#include "gvalue_enum.h"

#include "utils/gst-utils.h"

#include <glibmm.h>

GValueEnum::GValueEnum(GValue *gobj)
: GValueBase(gobj)
{}

gint GValueEnum::get_value() const
{
	return G_TYPE_IS_ENUM(G_VALUE_TYPE(g_value)) ? g_value_get_enum(g_value) : g_value_get_flags(g_value);
}

void GValueEnum::set_type(GstEnumType type)
{
	this->type = type;
}

template<typename T>
void append_to_vector(std::vector<std::pair<int, std::string>>& values, T *enum_values)
{
	int i = 0;

	while (enum_values[i].value_name)
	{
		values.push_back(std::make_pair(enum_values[i].value, enum_values[i].value_nick));
		i++;
	}
}

std::string GValueEnum::to_string() const
{
	if (G_VALUE_TYPE(g_value) == gst_utils_get_virtual_enum_type() || G_VALUE_TYPE(g_value) == gst_utils_get_virtual_flags_type())
	{
		gint value = get_value();
		if (!type)
		{
			return std::to_string(value);
		}
		try
		{
			return type.get().get_values().at(value);
		}
		catch (const std::out_of_range&)
		{
			return std::to_string(value);
		}
	}

	gpointer ptr = g_type_class_ref(G_VALUE_TYPE(g_value));
	if (G_IS_ENUM_CLASS(ptr))
	{
		Glib::Value_Enum<int> v;
		v.init(g_value);
		return G_ENUM_CLASS (ptr)->values[v.get()].value_nick;
	}
	else
	{
		Glib::Value_Flags<int> v;
		v.init(g_value);
		return G_FLAGS_CLASS (ptr)->values[v.get()].value_nick;
	}
}

std::vector<std::pair<int, std::string>> GValueEnum::get_values(GType type)
{
	std::vector<std::pair<int, std::string>> values;
	gpointer ptr = g_type_class_ref(type);

	if (G_IS_ENUM_CLASS (ptr))
	{
		append_to_vector(values, G_ENUM_CLASS (ptr)->values);
	}
	else
	{
		append_to_vector(values, G_FLAGS_CLASS (ptr)->values);
	}

	return values;
}

Gtk::Widget* GValueEnum::get_widget() const
{
	if (type && G_VALUE_TYPE(g_value) == gst_utils_get_virtual_enum_type())
	{
		Gtk::ComboBoxText *widget = Gtk::manage(new Gtk::ComboBoxText());
		gint pos = 0;
		gint value = get_value();
		for (auto entry : type.get().get_values())
		{
			widget->append(entry.second);
			if (entry.first == value)
			{
				widget->set_active(pos);
			}
			pos++;
		}

		return widget;
	}
	else if (type && G_VALUE_TYPE(g_value) == gst_utils_get_virtual_flags_type())
	{
		Gtk::Box *box = Gtk::manage(new Gtk::Box (Gtk::ORIENTATION_HORIZONTAL, 0));
		gint fv = get_value();
		auto values = type->get_values();
		for (auto val : values)
		{
			auto cb = Gtk::manage(new Gtk::CheckButton(val.second));
			cb->set_active(val.first & fv);
			cb->show();
			box->pack_start(*cb, false, 5);
		}

		return box;
	}
	else
	{
		Gtk::Entry *widget = Gtk::manage(new Gtk::Entry());
		widget->set_text(to_string());
		return widget;
	}
}
