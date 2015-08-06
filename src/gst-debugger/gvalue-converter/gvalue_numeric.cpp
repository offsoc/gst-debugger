/*
 * gvalue_numeric.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#include "gvalue_numeric.h"

#include <glibmm.h>

template<typename T>
GValueNumeric<T>::GValueNumeric(GValue *gobj)
: GValueBase(gobj)
{}

template<typename T>
std::string GValueNumeric<T>::to_string() const
{
	Glib::Value<T> v;
	v.init(g_value);
	return std::to_string(v.get());
}

template<typename T>
Gtk::Widget* GValueNumeric<T>::get_widget() const
{
	if (widget == nullptr)
	{
		widget = new Gtk::Entry();
	}
	dynamic_cast<Gtk::Entry*>(widget)->set_text(to_string());
	return widget;
}

// Check GCC
#if __GNUC__
#if !(__x86_64__ || __ppc64__)
template class GValueNumeric<gint64>;
template class GValueNumeric<guint64>;
#else

// todo glib/glibmm bug - report/fix it
template<>
std::string GValueNumeric<gint64>::to_string() const
{
	return std::to_string((G_VALUE_TYPE(g_value) == G_TYPE_INT64) ? g_value_get_int64(g_value) : g_value_get_long(g_value));
}

template<>
std::string GValueNumeric<guint64>::to_string() const
{
	return std::to_string((G_VALUE_TYPE(g_value) == G_TYPE_UINT64) ? g_value_get_uint64(g_value) : g_value_get_ulong(g_value));
}
#endif
#else
#error unsupported compiler
#endif
template class GValueNumeric<gfloat>;
template class GValueNumeric<gdouble>;
template class GValueNumeric<gint>;
template class GValueNumeric<guint>;
template class GValueNumeric<glong>;
template class GValueNumeric<gulong>;
