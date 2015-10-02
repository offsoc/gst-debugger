/*
 * gvalue_numeric.h
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_NUMERIC_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_NUMERIC_H_

#include "gvalue_base.h"

template<typename T>
class GValueNumeric : public GValueBase
{
	void update_value(const T &val);
	Gtk::Widget* create_widget() override;
	void update_widget(Gtk::Widget* widget) override;

public:
	GValueNumeric(GValue* gobj);

	std::string to_string() const override;
};


#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_NUMERIC_H_ */
