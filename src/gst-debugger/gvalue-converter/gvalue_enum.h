/*
 * gvalue_enum.h
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_ENUM_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_ENUM_H_

#include "gvalue_base.h"

#include <vector>
#include <utility>

class GValueEnum : public GValueBase
{
public:
	GValueEnum(GValue* gobj);

	std::string to_string() const override;

	static std::vector<std::pair<int, std::string>> get_values(GType type);

	Gtk::Widget* get_widget() const override;
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_ENUM_H_ */
