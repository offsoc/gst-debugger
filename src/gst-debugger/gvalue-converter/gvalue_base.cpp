/*
 * gvalue_base.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#include "gvalue_base.h"
#include "gvalue_unknown.h"
#include "gvalue_numeric.h"
#include "gvalue_boolean.h"
#include "gvalue_caps.h"
#include "gvalue_string.h"
#include "gvalue_enum.h"
#include "gvalue_flags.h"

#include <gst/gst.h>

GValueBase::GValueBase(GValue *gobj)
: g_value(gobj)
{
}

GValueBase::~GValueBase()
{
	clear_gvalue();
}

void GValueBase::clear_gvalue()
{
	if (g_value != nullptr)
	{
		g_value_unset(g_value);
		delete g_value;
	}
}

GValueBase* GValueBase::build_gvalue(GValue* gobj)
{
	GType value_type = G_VALUE_TYPE(gobj);
	switch (value_type)
	{
	case G_TYPE_STRING:
		return new GValueString(gobj);
	case G_TYPE_BOOLEAN:
		return new GValueBoolean(gobj);
	case G_TYPE_INT:
		return new GValueNumeric<gint>(gobj);
	case G_TYPE_UINT:
		return new GValueNumeric<guint>(gobj);
	case G_TYPE_LONG:
		return new GValueNumeric<glong>(gobj);
	case G_TYPE_ULONG:
		return new GValueNumeric<gulong>(gobj);
	case G_TYPE_INT64:
		return new GValueNumeric<gint64>(gobj);
	case G_TYPE_UINT64:
		return new GValueNumeric<guint64>(gobj);
	case G_TYPE_FLOAT:
		return new GValueNumeric<gfloat>(gobj);
	case G_TYPE_DOUBLE:
		return new GValueNumeric<gdouble>(gobj);
	default:
		break;
	}

	if (G_TYPE_IS_ENUM(G_VALUE_TYPE(gobj)))
	{
		return new GValueEnum(gobj);
	}

	if (G_TYPE_IS_FLAGS(G_VALUE_TYPE(gobj)))
	{
		return new GValueFlags(gobj);
	}


	if (G_VALUE_TYPE(gobj) == GST_TYPE_CAPS)
	{
		return new GValueCaps(gobj);
	}

	return new GValueUnknown(gobj);
}

void GValueBase::destroy_widget(GtkWidget *object, gpointer user_data)
{
	GValueBase *val = reinterpret_cast<GValueBase*>(user_data);

	val->widgets.erase(std::remove(val->widgets.begin(),
			val->widgets.end(), Glib::wrap(object)), val->widgets.end());
}

Gtk::Widget* GValueBase::get_widget()
{
	Gtk::Widget* widget = Gtk::manage(create_widget());

	g_signal_connect((gpointer)widget->gobj(), "destroy",
			(GCallback)&GValueBase::destroy_widget, (gpointer)this);

	widgets.push_back(widget);

	widget->set_data("is-gvalue-widget", GINT_TO_POINTER(1));
	update_widget(widget);

	return widget;
}

void GValueBase::update_gvalue(GValue* gobj)
{
	if (gobj != g_value)
	{
		clear_gvalue();
		g_value = gobj;
	}

	for (auto widget : widgets)
		update_widget(widget);
}

void GValueBase::set_sensitive(bool sensitive)
{
	for (auto widget : widgets)
		widget->set_sensitive(sensitive);
}
