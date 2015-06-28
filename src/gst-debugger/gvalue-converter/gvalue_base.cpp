/*
 * gvalue_base.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#include "gvalue_base.h"
#include "gvalue_boolean.h"
#include "gvalue_enum.h"
#include "gvalue_numeric.h"
#include "gvalue_string.h"

GValueBase::GValueBase(GValue *gobj)
: g_value(gobj)
{
}

GValueBase::~GValueBase()
{
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

	/*
	todo
	if (G_VALUE_TYPE(gobj) == gst_caps_get_type())
	{
		return new GValueCaps(param_spec);
	}
	*/
	return nullptr;
}
