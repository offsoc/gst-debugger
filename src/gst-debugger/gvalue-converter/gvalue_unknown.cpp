/*
 * gvalue_unknown.cpp
 *
 *  Created on: Oct 1, 2015
 *      Author: loganek
 */

#include "gvalue_unknown.h"

#include "../../common/gst-utils.h"

#include <glibmm/i18n.h>

GValueUnknown::GValueUnknown(GValue *gobj)
: GValueBase(gobj)
{}

std::string GValueUnknown::to_string() const
{
    std::string type_name;
    if (!G_VALUE_HOLDS_BOXED(g_value))
    {
        type_name = "INVALID";
        g_warning("GValueUnknown should hold boxed type!");
    }
    else
    {
        GstUnknownType *unknown_type = (GstUnknownType*)g_value_get_boxed(g_value);
        type_name = unknown_type->type_name;
    }

    return _("unsupported type: ") + type_name;
}

Gtk::Widget* GValueUnknown::create_widget()
{
	auto lbl = new Gtk::Label();

	lbl->set_text(to_string());

	return lbl;
}

