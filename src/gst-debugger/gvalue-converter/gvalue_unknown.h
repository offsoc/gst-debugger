/*
 * gvalue_unknown.h
 *
 *  Created on: Oct 1, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_UNKNOWN_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_UNKNOWN_H_

#include "gvalue_base.h"

class GValueUnknown : public GValueBase
{
protected:
	Gtk::Widget* create_widget() override;

	void update_widget(Gtk::Widget* widget) override{}

public:
	GValueUnknown(GValue* gobj);

	std::string to_string() const override;
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_UNKNOWN_H_ */
