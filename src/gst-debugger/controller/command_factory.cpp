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

GstDebugger::HookRequest* CommandFactory::create_pad_hook_request(bool enable, const std::string &pad_path)
{
	auto *pad_request = new GstDebugger::PadHookRequest();
	pad_request->set_pad(pad_path);
	auto hook_request = create_hook_request(enable);
	hook_request->set_allocated_pad_hook(pad_request);
	return hook_request;
}

GstDebugger::HookRequest* CommandFactory::create_hook_request(bool enable)
{
	auto *request = new GstDebugger::HookRequest();
	request->set_action(enable ? GstDebugger::ADD : GstDebugger::REMOVE);
	return request;
}

void CommandFactory::send_query_request_command(bool enable, const std::string &pad_path, int type)
{
	auto *request = create_pad_hook_request(enable, pad_path);
	request->mutable_pad_hook()->mutable_query()->set_type(type);

	GstDebugger::Command cmd;
	cmd.set_allocated_hook_request(request);

	client->send_command(cmd);
}

void CommandFactory::send_event_request_command(bool enable, const std::string &pad_path, int type)
{
	auto *request = create_pad_hook_request(enable, pad_path);
	request->mutable_pad_hook()->mutable_event()->set_type(type);

	GstDebugger::Command cmd;
	cmd.set_allocated_hook_request(request);

	client->send_command(cmd);
}

void CommandFactory::send_buffer_request_command(bool enable, const std::string &pad_path, bool send_data)
{
	auto *request = create_pad_hook_request(enable, pad_path);
	request->mutable_pad_hook()->mutable_buffer()->set_send_data(send_data);

	GstDebugger::Command cmd;
	cmd.set_allocated_hook_request(request);

	client->send_command(cmd);

	client->send_command(cmd);
}

void CommandFactory::send_message_request_command(int message_type, bool enable)
{
	auto msg_request = new GstDebugger::MessageRequest();
	msg_request->set_type(message_type);

	auto hook_request = create_hook_request(enable);
	hook_request->set_allocated_message(msg_request);

	GstDebugger::Command cmd;
	cmd.set_allocated_hook_request(hook_request);

	client->send_command(cmd);
}

void CommandFactory::send_set_threshold_command(const std::string &threshold_list, bool overwrite)
{
	GstDebugger::Command cmd;
	// todo overwrite
	cmd.set_log_threshold(threshold_list);
	client->send_command(cmd);
}

void CommandFactory::send_set_log_hook_command(bool enable, const std::string &category, int log_level)
{
	auto log_request = new GstDebugger::LogRequest();
	log_request->set_level(log_level);
	log_request->set_category(category);

	auto hook_request = create_hook_request(enable);
	hook_request->set_allocated_log(log_request);

	GstDebugger::Command cmd;
	cmd.set_allocated_hook_request(hook_request);

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

void CommandFactory::send_set_property_command(const std::string &path, const std::string &property_name, GValue *gvalue)
{
	GstDebugger::Value *value = new GstDebugger::Value();
	GType gtype; InternalGType internal_type;
	char *data = g_value_serialize(gvalue, &gtype, &internal_type);
	value->set_data(data);
	g_free (data);
	value->set_gtype(gtype);
	value->set_internal_type(internal_type);
	value->set_type_name("xxx"); // todo

	GstDebugger::PropertyValue *prop_val = new GstDebugger::PropertyValue();
	prop_val->set_object(path);
	prop_val->set_name(property_name);
	prop_val->set_allocated_value(value);

	GstDebugger::Command cmd;
	cmd.set_allocated_property_set(prop_val);

	client->send_command(cmd);

}

void CommandFactory::send_request_pad_dynamic_info(const std::string &pad_path)
{
	GstDebugger::Command cmd;
	cmd.set_pad_dynamic_info(pad_path);
	client->send_command(cmd);
}
