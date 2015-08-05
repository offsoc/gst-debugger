/*
 * gvalue_base.h
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_BASE_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_BASE_H_

#include <glib-object.h>

#include <gtkmm.h>

#include <string>

class GValueBase
{
protected:
	GValue* g_value;

	virtual void update_widget() {}
public:
	GValueBase(GValue* gobj);
	virtual ~GValueBase();

	virtual std::string to_string() const = 0;

	virtual Gtk::Widget* get_widget() const = 0;

	void update_gvalue(const std::shared_ptr<GValueBase> &gvalue);

	static GValueBase* build_gvalue(GValue* g_value);
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_BASE_H_ */
