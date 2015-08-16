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

enum class StringDataFormat
{
	BINARY = 0,
	OCT = 1,
	DEC = 2,
	HEX = 3
};

std::string get_presence_str(Gst::PadPresence p);

std::string get_direction_str(Gst::PadDirection d);

void display_template_info(const Glib::RefPtr<Gst::PadTemplate> &tpl, const Glib::RefPtr<Gtk::TreeStore> &model,
		const Gtk::TreeModelColumn<Glib::ustring> &col_name,
		const Gtk::TreeModelColumn<Glib::ustring> &col_value,
		boost::optional<const Gtk::TreeModel::Row&> parent_row = boost::none);

void display_caps(const Glib::RefPtr<Gst::Caps> &caps,
		const Glib::RefPtr<Gtk::TreeStore> &model, const Gtk::TreeModelColumn<Glib::ustring> &col_name,
		const Gtk::TreeModelColumn<Glib::ustring> &col_value, const Gtk::TreeModel::Row& parent_row);

Glib::RefPtr<Gst::PadTemplate> protocol_template_to_gst_template(const TopologyTemplate &tpl);

std::string flags_value_to_string(guint value);

std::string buffer_data_to_string(StringDataFormat format, const Glib::RefPtr<Gst::Buffer> &buffer, gsize max_size, int columns_in_row);

Glib::RefPtr<Gtk::Builder> get_dialog_ui_def();

template<typename T>
T* load_dialog(const std::string &name)
{
	T* tmp = nullptr;
	Glib::RefPtr<Gtk::Builder> dialogs_builder = get_dialog_ui_def();
	dialogs_builder->get_widget_derived(name, tmp);
	return tmp;
}

#endif /* SRC_GST_DEBUGGER_UI_UTILS_H_ */
