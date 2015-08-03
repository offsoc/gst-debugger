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
public:
	GValueCaps(GValue* gobj);

	std::string to_string() const override;

	Gtk::Widget* get_widget() const override;
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_CAPS_H_ */
