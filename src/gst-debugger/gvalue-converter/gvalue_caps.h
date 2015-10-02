/*
 * gvalue_caps.h
 *
 *  Created on: Aug 3, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_CAPS_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_CAPS_H_

#include "gvalue_base.h"

class GValueCaps : public GValueBase
{
	Gtk::Widget* create_widget() override;

	void update_widget(Gtk::Widget* widget) override;

public:
	GValueCaps(GValue* gobj);

	std::string to_string() const override;
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_CAPS_H_ */
