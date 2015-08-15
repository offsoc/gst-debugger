/*
 * gst_enum_container.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_GST_ENUM_MODEL_H_
#define SRC_GST_DEBUGGER_GST_ENUM_MODEL_H_

#include <glib.h>

#include <map>
#include <vector>
#include <string>

class GstEnumType
{
	struct ValueInfo { std::string name; std::string nick; };
	std::map<int, ValueInfo> values;
	std::string type_name;
	GType gtype;

public:
	GstEnumType(const std::string &type_name, GType gtype) : type_name(type_name), gtype(gtype) {}
	std::string get_name() const { return type_name; }
	GType get_gtype() const { return gtype; }
	void add_value(const std::string& name, int value, const std::string &nick)
	{
		values[value] = ValueInfo {name, nick};
	}
	const std::map<int, ValueInfo>& get_values() const { return values; }
};

#endif /* SRC_GST_DEBUGGER_GST_ENUM_MODEL_H_ */
