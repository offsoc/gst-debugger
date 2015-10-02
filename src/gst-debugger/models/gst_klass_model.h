/*
 * gst_klass_model.h
 *
 *  Created on: Oct 1, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODELS_GST_KLASS_MODEL_H_
#define SRC_GST_DEBUGGER_MODELS_GST_KLASS_MODEL_H_

#include <string>

class PropertyModel
{
	std::string name;
	std::string nick;
	std::string blurb;
	GParamFlags flags;

public:
	PropertyModel(const std::string &name, const std::string &nick, const std::string &blurb, GParamFlags flags)
	: name(name), nick(nick), blurb(blurb), flags(flags) {}

	std::string get_name() const { return name; }
	std::string get_nick() const { return nick; }
	std::string get_blurb() const { return blurb; }
	GParamFlags get_flags() const { return flags; }
};

class KlassModel
{
	std::string name;
	std::vector<PropertyModel> properties;

public:
	KlassModel(const std::string &klass_name)
	: name (klass_name)
	{}

	std::string get_name() const { return name; }

	void append_property(const PropertyModel& property)
	{
		// todo copy&paste
		auto it = std::find_if(properties.begin(), properties.end(), [this](const PropertyModel& model) { return model.get_name() == name; });

		if (it == properties.end())
			properties.push_back(property);
		else
			*it = property;
	}

	const std::vector<PropertyModel>& get_properties() const { return properties; }

	boost::optional<PropertyModel> get_property(const std::string &property_name) const
	{
		auto it = std::find_if(properties.begin(), properties.end(), [property_name](const PropertyModel& m) { return m.get_name() == property_name; });

		if (it == properties.end())
			return boost::none;

		return *it;
	}
};

#endif /* SRC_GST_DEBUGGER_MODELS_GST_KLASS_MODEL_H_ */
