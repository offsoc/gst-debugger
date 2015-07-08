/*
 * gst_bin_to_dot_converter.h
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_PIPELINE_DRAWER_GST_BIN_TO_DOT_CONVERTER_H_
#define SRC_GST_DEBUGGER_PIPELINE_DRAWER_GST_BIN_TO_DOT_CONVERTER_H_

#include <gstreamermm.h>

class GstBinToDotConverter
{
private:
	GString * str = nullptr;

	std::string debug_dump_make_object_name (const Glib::RefPtr<Gst::Object>& obj);
	void debug_dump_pad (const Glib::RefPtr<Gst::Pad>& pad, const gchar * color_name, const std::string& element_name, const gint indent);
	void debug_dump_element_pad (const Glib::RefPtr<Gst::Pad>& pad, const Glib::RefPtr<Gst::Element>& element, const gint indent);
	void debug_dump_element_pad_link (const Glib::RefPtr<Gst::Pad>& pad, Glib::RefPtr<Gst::Element>& element, const gint indent);
	void debug_dump_element_pads (Gst::Iterator<Gst::Pad> pad_iter, Glib::RefPtr<Gst::Element>& element, const gint indent);
	void debug_dump_element (const Glib::RefPtr<Gst::Bin>& bin, const gint indent);
	void debug_dump_header ();

public:
	std::string to_dot_data (const Glib::RefPtr<Gst::Bin>& bin);
};

#endif /* SRC_GST_DEBUGGER_PIPELINE_DRAWER_GST_BIN_TO_DOT_CONVERTER_H_ */
