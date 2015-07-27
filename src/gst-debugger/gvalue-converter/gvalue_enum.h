/*
 * gvalue_enum.h
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_ENUM_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_ENUM_H_

#include "gvalue_base.h"

#include "../models/gst_enum_model.h"

#include <boost/optional/optional.hpp>

#include <vector>
#include <utility>

class GValueEnum : public GValueBase
{
	boost::optional<GstEnumType> type;

public:
	GValueEnum(GValue* gobj);

	void set_type(GstEnumType type);

	std::string to_string() const override;

	gint get_value() const;

	static std::vector<std::pair<int, std::string>> get_values(GType type);

	Gtk::Widget* get_widget() const override;
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_ENUM_H_ */
