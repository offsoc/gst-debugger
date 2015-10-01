/*
 * command_factory.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "command_factory.h"
#include "common/common.h"
#include "common/serializer.h"

void CommandFactory::send_property_request_command(const std::string &element_path, const std::string &property_name)
{
	GstDebugger::Command cmd;
	GstDebugger::PropertyRequest *request = new GstDebugger::PropertyRequest();
	request->set_object(element_path);
	request->set_name(property_name);
	cmd.set_allocated_property(request);

	client->send_command(cmd);
}


void CommandFactory::send_request_entire_topology_command()
{
	GstDebugger::Command cmd;
	cmd.set_entire_topology(true);

	client->send_command(cmd);
}

GstDebugger::PadWatchRequest* CommandFactory::create_pad_watch_request(bool enable, const std::string &pad_path)
{
	GstDebugger::PadWatchRequest *request = new GstDebugger::PadWatchRequest();
	request->set_pad(pad_path);
	request->set_action(enable ? GstDebugger::ADD : GstDebugger::REMOVE);
	return request;
}

void CommandFactory::send_query_request_command(bool enable, const std::string &pad_path, int type)
{
	auto request = create_pad_watch_request(enable, pad_path);
	GstDebugger::QueryWatchRequest *ev_request = new GstDebugger::QueryWatchRequest();
	ev_request->set_type(type);
	request->set_allocated_query(ev_request);
	GstDebugger::Command cmd;
	cmd.set_allocated_pad_watch(request);

	client->send_command(cmd);
}

void CommandFactory::send_event_request_command(bool enable, const std::string &pad_path, int type)
{
	auto request = create_pad_watch_request(enable, pad_path);
	GstDebugger::EventWatchRequest *ev_request = new GstDebugger::EventWatchRequest();
	ev_request->set_type(type);
	request->set_allocated_event(ev_request);
	GstDebugger::Command cmd;
	cmd.set_allocated_pad_watch(request);

	client->send_command(cmd);
}

void CommandFactory::send_message_request_command(int message_type, bool enable)
{
	GstDebugger::MessageRequest *request = new GstDebugger::MessageRequest();
	request->set_type(message_type);
	request->set_action(enable ? GstDebugger::ADD : GstDebugger::REMOVE);
	GstDebugger::Command cmd;
	cmd.set_allocated_message(request);

	client->send_command(cmd);
}

void CommandFactory::send_set_threshold_command(const std::string &threshold_list, bool overwrite)
{
	GstDebugger::Command cmd;
	// todo overwrite
	cmd.set_log_threshold(threshold_list);
	client->send_command(cmd);
}

void CommandFactory::send_set_log_watch_command(bool enable, const std::string &category, int log_level)
{
	GstDebugger::Command cmd;
	GstDebugger::LogRequest *request = new GstDebugger::LogRequest();
	request->set_level(log_level);
	request->set_action(enable ? GstDebugger::ADD : GstDebugger::REMOVE);
	request->set_category(category);
	cmd.set_allocated_log(request);

	client->send_command(cmd);
}

void CommandFactory::send_data_type_request_command(const std::string &type_name, GstDebugger::TypeDescriptionRequest_Type type)
{
	GstDebugger::Command cmd;
	GstDebugger::TypeDescriptionRequest *request = new GstDebugger::TypeDescriptionRequest();
	request->set_type(type);
	request->set_name(type_name);
	cmd.set_allocated_type_description(request);

	client->send_command(cmd);
}

void CommandFactory::send_request_debug_categories_command()
{
	GstDebugger::Command cmd;
	cmd.set_debug_categories_list(true);

	client->send_command(cmd);
}


/*
void CommandFactory::send_request_topology_command()
{
	Command cmd;
	cmd.set_command_type(Command_CommandType_TOPOLOGY);

	client->send_command(cmd);
}

void CommandFactory::send_property_command(const std::string &path, const std::string &property_name, GValue *gvalue)
{
	Command cmd;
	cmd.set_command_type(Command_CommandType_PROPERTY);
	Property *property = new Property();
	property->set_element_path(path);
	property->set_property_name(property_name);
	GType type; InternalGType internal_type;
	gchar *serialized = g_value_serialize(gvalue, &type, &internal_type);
	property->set_property_value(serialized);
	g_free(serialized);
	property->set_internal_type(internal_type);
	property->set_type(type);
	cmd.set_allocated_property(property);

	client->send_command(cmd);
}

void CommandFactory::send_request_pad_dynamic_info(const std::string &pad_path)
{
	Command cmd;
	cmd.set_command_type(Command_CommandType_PAD_DYNAMIC_INFO);
	cmd.set_pad_path(pad_path);
	client->send_command(cmd);
}
*/
