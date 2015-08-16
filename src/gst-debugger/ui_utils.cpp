/*
 * ui_utils.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: loganek
 */

#include "ui_utils.h"
#include "gst-debugger-dialogs.ui.h"

#include <iomanip>
#include <bitset>

std::string get_presence_str(Gst::PadPresence p)
{
	std::string presence;
	switch (p)
	{
	case Gst::PAD_ALWAYS: presence = "ALWAYS"; break;
	case Gst::PAD_SOMETIMES: presence = "SOMETIMES"; break;
	case Gst::PAD_REQUEST: presence = "REQUEST"; break;
	default: presence = "UNKNOWN";
	}
	return presence;
}

std::string get_direction_str(Gst::PadDirection d)
{
	std::string direction;
	switch (d)
	{
	case Gst::PAD_SINK: direction = "SINK"; break;
	case Gst::PAD_SRC: direction = "SRC"; break;
	default: direction = "UNKNOWN";
	}
	return direction;
}

#define APPEND_SUB_ROW(name, value, parent) \
	[=] { \
		auto childrow = *(model->append(parent.children())); \
		childrow[col_name] = name; \
		childrow[col_value] = value; \
		return childrow; \
	} ()

void display_template_info(const Glib::RefPtr<Gst::PadTemplate> &tpl,
		const Glib::RefPtr<Gtk::TreeStore> &model, const Gtk::TreeModelColumn<Glib::ustring> &col_name,
		const Gtk::TreeModelColumn<Glib::ustring> &col_value, boost::optional<const Gtk::TreeModel::Row&> parent_row)
{
	Gtk::TreeRow row;
	if (parent_row)
	{
		row = APPEND_SUB_ROW("Template", tpl->get_name_template(), parent_row.get());
	}
	else
	{
		row = *(model->append());
		row[col_name] = "Template";
		row[col_value] = tpl->get_name_template();
	}
	APPEND_SUB_ROW("Presence", get_presence_str(tpl->get_presence()), row);
	APPEND_SUB_ROW("Direction", get_direction_str(tpl->get_direction()), row);
	APPEND_SUB_ROW("Caps", tpl->get_caps()->to_string(), row);
}

#undef APPEND_SUB_ROW

Glib::RefPtr<Gst::PadTemplate> protocol_template_to_gst_template(const TopologyTemplate &tpl)
{
	return Gst::PadTemplate::create(tpl.name_template(),
			static_cast<Gst::PadDirection>(tpl.direction()),
			static_cast<Gst::PadPresence>(tpl.presence()),
			Gst::Caps::create_from_string(tpl.caps()));
}

std::string flags_value_to_string(guint value)
{
	char buff[12];
	g_snprintf(buff, 12u, "0x%08x", value);
	return buff;
}

std::string buffer_data_to_string(StringDataFormat format, const Glib::RefPtr<Gst::Buffer> &buffer, gsize max_size, int columns_in_row)
{
	std::ostringstream ss;
	gsize display_size = std::min(max_size, buffer->get_size());
	Glib::RefPtr<Gst::MapInfo> map_info(new Gst::MapInfo());

	ss << ((format == StringDataFormat::HEX) ?
			std::hex : format == StringDataFormat::OCT ? std::oct : std::dec)
					<< std::setfill('0');

	int width = format == StringDataFormat::HEX ? 2 : format == StringDataFormat::BINARY ? 8 : 3;

	buffer->map(map_info, Gst::MAP_READ);

	for (std::size_t i = 0; i < display_size; i++)
	{
		if (i != 0 && i % columns_in_row == 0)
			ss << std::endl;
		if (format == StringDataFormat::BINARY)
		{
			ss << std::setw(width) << std::bitset<8>(map_info->get_data()[i]) << " ";
		}
		else
		{
			ss << std::setw(width) << static_cast<int>(map_info->get_data()[i]) << " ";
		}
	}

	if (display_size < buffer->get_size())
	{
		ss << std::endl << "more...";
	}

	buffer->unmap(map_info);

	return ss.str();
}

Glib::RefPtr<Gtk::Builder> get_dialog_ui_def()
{
	return Gtk::Builder::create_from_string(std::string((char*)gst_debugger_dialogs_glade, gst_debugger_dialogs_glade_len));
}
