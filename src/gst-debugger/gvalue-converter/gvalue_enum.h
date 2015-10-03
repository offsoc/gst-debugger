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

#include <mutex>
#include <vector>
#include <utility>

class GValueEnum : public GValueBase
{
	boost::optional<GstEnumType> type;
	std::mutex emit_m;

	Gtk::Widget* create_widget() override;

	void update_widget(Gtk::Widget* widget) override;

public:
	GValueEnum(GValue* gobj);

	void set_type(GstEnumType type) { this->type = type; }

	std::string to_string() const override;

	gint get_value() const;
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_ENUM_H_ */
