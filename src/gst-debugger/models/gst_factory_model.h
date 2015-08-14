/*
 * gst_factory_model.h
 *
 *  Created on: Aug 13, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODELS_GST_FACTORY_MODEL_H_
#define SRC_GST_DEBUGGER_MODELS_GST_FACTORY_MODEL_H_

#include <gstreamermm.h>

#include <vector>
#include <map>

class FactoryModel
{
	std::string name;
	std::vector<Glib::RefPtr<Gst::PadTemplate>> pad_templates;
	std::map<std::string, std::string> meta;

public:
	FactoryModel(const std::string &factory_name)
	: name (factory_name)
	{}

	void append_template(const Glib::RefPtr<Gst::PadTemplate> &tpl);

	void append_meta(const std::string &key, const std::string &value) { meta[key] = value; }

	std::string get_name() const { return name; }

	std::vector<Glib::RefPtr<Gst::PadTemplate>> get_pad_templates() const { return pad_templates; }

	std::map<std::string, std::string> get_metadata() const { return meta; }
};

#endif /* SRC_GST_DEBUGGER_MODELS_GST_FACTORY_MODEL_H_ */
