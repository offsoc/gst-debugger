/*
 * gvalue_flags.h
 *
 *  Created on: Oct 2, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_FLAGS_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_FLAGS_H_

#include "gvalue_base.h"

#include "../models/gst_enum_model.h"

#include <boost/optional/optional.hpp>
#include <mutex>

#include <vector>
#include <utility>

class GValueFlags : public GValueBase
{
	std::mutex emit_m;
	boost::optional<GstEnumType> type;

	Gtk::Widget* create_widget() override;

	void update_widget(Gtk::Widget* widget) override;

	Gtk::Box* get_internal_box(Gtk::Widget* widget);

public:
	GValueFlags(GValue* gobj);

	void set_type(GstEnumType type) { this->type = type; }

	std::string to_string() const override;

	gint get_value() const;

	void set_sensitive(bool sensitive) override;

	static std::vector<std::pair<int, std::string>> get_values(GType type);
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_FLAGS_H_ */
