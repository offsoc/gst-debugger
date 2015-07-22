/*
 * gst_enum_container.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_GST_ENUM_CONTAINER_H_
#define SRC_GST_DEBUGGER_GST_ENUM_CONTAINER_H_

#include "frame_receiver.h"

class GstEnumType
{
	std::map<std::string, int> values;
	std::string type_name;

public:
	GstEnumType(const std::string &type_name) : type_name(type_name) {}
	std::string get_type_name() const { return type_name; }
	void add_value(const std::string& name, int value)
	{
		values[name] = value;
	}
};

class GstEnumContainer : public FrameReceiver
{
	std::vector<GstEnumType> types;

	std::vector<GstEnumType>::iterator get_enum_type_it(const std::string &type_name);

protected:
	void process_frame() override;
public:
	bool has_type(const std::string &type_name);
	GstEnumType get_type(const std::string &type_name);
};

#endif /* SRC_GST_DEBUGGER_GST_ENUM_CONTAINER_H_ */
