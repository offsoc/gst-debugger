/*
 * command_factory.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "command_factory.h"
#include "protocol/common.h"
#include "protocol/serializer.h"

void CommandFactory::send_pad_watch_command(bool enable, PadWatch_WatchType watch_type, const std::string &pad_path, int qe_type)
{
	Command cmd;
	PadWatch *pad_watch = new PadWatch();
	pad_watch->set_toggle(enable ? ENABLE : DISABLE);
	pad_watch->set_watch_type(watch_type);
	pad_watch->set_pad_path(pad_path);
	pad_watch->set_qe_type(qe_type);
	cmd.set_command_type(Command_CommandType_PAD_WATCH);
	cmd.set_allocated_pad_watch(pad_watch);

	client->send_command(cmd);
}

void CommandFactory::send_property_request_command(const std::string &element_path, const std::string &property_name)
{
	Command cmd;
	Property *property = new Property();
	property->set_element_path(element_path);
	property->set_property_name(property_name);
	cmd.set_command_type(Command_CommandType_PROPERTY);
	cmd.set_allocated_property(property);
	client->send_command(cmd);
}

void CommandFactory::send_message_request_command(int message_type, bool enable)
{
	MessageWatch *msg_watch = new MessageWatch();
	msg_watch->set_message_type(message_type);
	msg_watch->set_toggle(enable ? ENABLE : DISABLE);
	Command cmd;
	cmd.set_command_type(Command_CommandType_MESSAGE_WATCH);
	cmd.set_allocated_message_watch(msg_watch);

	client->send_command(cmd);
}

void CommandFactory::send_set_threshold_command(const std::string &threshold_list, bool overwrite)
{
	Command cmd;
	LogThreshold *log_threshold = new LogThreshold();
	log_threshold->set_list(threshold_list);
	log_threshold->set_overwrite(overwrite);
	cmd.set_command_type(Command_CommandType_LOG_THRESHOLD);
	cmd.set_allocated_log_threshold(log_threshold);

	client->send_command(cmd);
}

void CommandFactory::send_set_log_watch_command(bool enable, int log_level)
{
	Command cmd;
	LogWatch *log_watch = new LogWatch();
	log_watch->set_toggle(enable ? ENABLE : DISABLE);
	log_watch->set_log_level(log_level);
	cmd.set_command_type(Command_CommandType_LOG_WATCH);
	cmd.set_allocated_log_watch(log_watch);

	client->send_command(cmd);
}

void CommandFactory::send_request_debug_categories_command()
{
	Command cmd;
	cmd.set_command_type(Command_CommandType_DEBUG_CATEGORIES);

	client->send_command(cmd);
}

void CommandFactory::send_request_topology_command()
{
	Command cmd;
	cmd.set_command_type(Command_CommandType_TOPOLOGY);

	client->send_command(cmd);
}

void CommandFactory::send_enum_type_request_command(const std::string &enum_name)
{
	Command cmd;
	cmd.set_command_type(Command_CommandType_ENUM_TYPE);
	cmd.set_enum_name(enum_name);

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
