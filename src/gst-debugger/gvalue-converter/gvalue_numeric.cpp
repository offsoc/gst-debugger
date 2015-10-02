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
void GValueNumeric<T>::update_value(const T &val)
{
	Glib::Value<T> v;
	v.init(g_value);
	v.set(val);
	g_value_reset(this->g_value);
	g_value_copy(v.gobj(), this->g_value);

	update_gvalue(this->g_value);
}

template<typename T>
void GValueNumeric<T>::update_widget(Gtk::Widget* widget)
{
	dynamic_cast<Gtk::Entry*>(widget)->set_text(to_string());
}

template<typename T>
Gtk::Widget* GValueNumeric<T>::create_widget()
{
	auto entry = new Gtk::Entry();
	entry->signal_activate().connect([this, entry]{
		update_value((T)std::atol(entry->get_text().c_str()));
	});

	entry->signal_activate().connect(widget_value_changed);
	update_widget(entry);

	return entry;
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

template<>
void GValueNumeric<gint64>::update_value(const gint64 &val)
{
	if (G_VALUE_TYPE(g_value) == G_TYPE_INT64)
		g_value_set_int64(g_value, val);
	else
		g_value_set_long(g_value, val);
}

template<>
void GValueNumeric<guint64>::update_value(const guint64 &val)
{
	if (G_VALUE_TYPE(g_value) == G_TYPE_UINT64)
		g_value_set_uint64(g_value, val);
	else
		g_value_set_ulong(g_value, val);
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
