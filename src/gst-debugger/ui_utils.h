/*
 * ui_utils.h
 *
 *  Created on: Aug 13, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_UI_UTILS_H_
#define SRC_GST_DEBUGGER_UI_UTILS_H_

#include "protocol/gstdebugger.pb.h"

#include <gstreamermm.h>
#include <gtkmm.h>

#include <boost/optional/optional.hpp>

std::string get_presence_str(Gst::PadPresence p);

std::string get_direction_str(Gst::PadDirection d);

void display_template_info(const Glib::RefPtr<Gst::PadTemplate> &tpl, const Glib::RefPtr<Gtk::TreeStore> &model,
		const Gtk::TreeModelColumn<Glib::ustring> &col_name,
		const Gtk::TreeModelColumn<Glib::ustring> &col_value,
		boost::optional<const Gtk::TreeModel::Row&> parent_row = boost::none);

Glib::RefPtr<Gst::PadTemplate> protocol_template_to_gst_template(const TopologyTemplate &tpl);

std::string flags_value_to_string(guint value);

#endif /* SRC_GST_DEBUGGER_UI_UTILS_H_ */
