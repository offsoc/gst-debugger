/*
 * command_factory.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_COMMAND_FACTORY_H_
#define SRC_GST_DEBUGGER_CONTROLLER_COMMAND_FACTORY_H_

#include "base_controller.h"

#include "common/gstdebugger.pb.h"

#include <boost/optional/optional.hpp>

class CommandFactory : public virtual BaseController
{
	static GstDebugger::HookRequest* create_pad_hook_request(bool enable, const std::string &pad_path);
	static GstDebugger::HookRequest* create_hook_request(bool enable);

public:
	void send_property_request_command(const std::string &element_path, const std::string &property_name);
	void send_event_request_command(bool enable, const std::string &pad_path, int type);
	void send_query_request_command(bool enable, const std::string &pad_path, int type);
	void send_buffer_request_command(bool enable, const std::string &pad_path, bool send_data);
	void send_message_request_command(int message_type, bool enable);
	void send_set_threshold_command(const std::string &threshold_list, bool overwrite);
	void send_set_log_hook_command(bool enable, const std::string &category, int log_level);
	void send_data_type_request_command(const std::string &type_name, GstDebugger::TypeDescriptionRequest_Type type);
	void send_request_debug_categories_command();
	void send_request_entire_topology_command();
	void send_set_property_command(const std::string &path, const std::string &property_name, GValue *gvalue);
	/*void send_request_pad_dynamic_info(const std::string &pad_path);*/
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_COMMAND_FACTORY_H_ */
