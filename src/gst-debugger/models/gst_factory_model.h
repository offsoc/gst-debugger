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

// todo the same as EnumContainer?
class GstFactoryContainer
{
	std::vector<FactoryModel> factories;

	std::vector<FactoryModel>::iterator get_factory_it(const std::string &factory_name);

public:
	typedef std::vector<FactoryModel>::iterator iterator;
	typedef std::vector<FactoryModel>::const_iterator const_iterator;

	void update_factory(const FactoryModel &type);
	bool has_factory(const std::string &type_name);
	FactoryModel get_factory(const std::string &factory_name);

	iterator begin() { return factories.begin(); }
	const_iterator begin() const { return factories.begin(); }
	iterator end() { return factories.end(); }
	const_iterator end() const { return factories.end(); }
};

#endif /* SRC_GST_DEBUGGER_MODELS_GST_FACTORY_MODEL_H_ */
