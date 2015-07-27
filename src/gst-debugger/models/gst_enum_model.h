/*
 * gst_enum_container.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_GST_ENUM_MODEL_H_
#define SRC_GST_DEBUGGER_GST_ENUM_MODEL_H_

#include <map>
#include <vector>
#include <string>

class GstEnumType
{
	std::map<int, std::string> values;
	std::string type_name;

public:
	GstEnumType(const std::string &type_name) : type_name(type_name) {}
	std::string get_type_name() const { return type_name; }
	void add_value(const std::string& name, int value)
	{
		values[value] = name;
	}
	const std::map<int, std::string>& get_values() const { return values; }
};

class GstEnumContainer
{
	std::vector<GstEnumType> types;
	std::vector<GstEnumType>::iterator get_enum_type_it(const std::string &type_name);

public:
	typedef std::vector<GstEnumType>::iterator iterator;
	typedef std::vector<GstEnumType>::const_iterator const_iterator;

	void update_type(const GstEnumType &type);
	bool has_type(const std::string &type_name);
	GstEnumType get_type(const std::string &type_name);

	iterator begin() { return types.begin(); }
	const_iterator begin() const { return types.begin(); }
	iterator end() { return types.end(); }
	const_iterator end() const { return types.end(); }
};

#endif /* SRC_GST_DEBUGGER_GST_ENUM_MODEL_H_ */
