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
#include <vector>

class GValueBase
{
	void clear_gvalue();

protected:
	GValue* g_value;
	mutable std::vector<Gtk::Widget*> widgets;

	virtual Gtk::Widget *create_widget() = 0;

	virtual void update_widget(Gtk::Widget* widget) = 0;

	static void destroy_widget(GtkWidget *object, gpointer user_data);

public:
	GValueBase(GValue* gobj);
	virtual ~GValueBase();

	virtual std::string to_string() const = 0;

	/*
	 * caller should manage returned object
	 * by adding it to the container
	 * */
	Gtk::Widget* get_widget();

	static GValueBase* build_gvalue(GValue* g_value);

	GValue* get_gvalue() const { return g_value; }

	void update_gvalue(GValue* gobj);

	sigc::signal<void> widget_value_changed;

	virtual void set_sensitive(bool sensitive);
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_BASE_H_ */
