/*
 * gst_bin_to_dot_converter.h
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_PIPELINE_DRAWER_GST_BIN_TO_DOT_CONVERTER_H_
#define SRC_GST_DEBUGGER_PIPELINE_DRAWER_GST_BIN_TO_DOT_CONVERTER_H_

#include "../models/gst_pipeline_model.h"

class GstBinToDotConverter
{
private:
	std::string str;

	std::string make_object_name(const GraphObjectPtr& obj);
	void dump_pad(const GraphPadPtr& pad);
	void dump_pad_link(const GraphPadPtr& pad);
	void dump_element(const GraphElementPtr& bin);
	void dump_header();
	void dump_top_pads(const GraphElementPtr& bin);

public:
	std::string to_dot_data (const GraphElementPtr& bin);
};

#endif /* SRC_GST_DEBUGGER_PIPELINE_DRAWER_GST_BIN_TO_DOT_CONVERTER_H_ */
