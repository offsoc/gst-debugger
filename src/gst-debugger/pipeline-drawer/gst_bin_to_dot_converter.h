/*
 * gst_bin_to_dot_converter.h
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_PIPELINE_DRAWER_GST_BIN_TO_DOT_CONVERTER_H_
#define SRC_GST_DEBUGGER_PIPELINE_DRAWER_GST_BIN_TO_DOT_CONVERTER_H_

#include "graph_elements.h"

class GstBinToDotConverter
{
private:
	GString * str = nullptr;

	std::string debug_dump_make_object_name (const std::shared_ptr<GraphObject>& obj);
	void debug_dump_pad (const std::shared_ptr<GraphPad>& pad, const gchar * color_name, const std::string& element_name, const gint indent);
	void debug_dump_element_pad (const std::shared_ptr<GraphPad>& pad, const std::shared_ptr<GraphElement>& element, gint indent);
	void debug_dump_element_pad_link (const std::shared_ptr<GraphPad>& pad, const gint indent);
	void debug_dump_element_pads (const std::vector<std::shared_ptr<GraphPad>>& pads, const std::shared_ptr<GraphElement>& element, const gint indent);
	void debug_dump_element (const std::shared_ptr<GraphElement>& bin, const gint indent);
	void debug_dump_header ();
	void debug_dump_top_pads(const std::shared_ptr<GraphElement>& bin);

public:
	std::string to_dot_data (const std::shared_ptr<GraphElement>& bin);
};

#endif /* SRC_GST_DEBUGGER_PIPELINE_DRAWER_GST_BIN_TO_DOT_CONVERTER_H_ */
