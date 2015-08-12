/*
 * command_factory.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_COMMAND_FACTORY_H_
#define SRC_GST_DEBUGGER_CONTROLLER_COMMAND_FACTORY_H_

#include "base_controller.h"

#include "protocol/gstdebugger.pb.h"

class CommandFactory : public virtual BaseController
{
public:
	void send_pad_watch_command(bool enable, PadWatch_WatchType watch_type, const std::string &pad_path, int qe_type);
	void send_property_request_command(const std::string &element_path, const std::string &property_name);
	void send_message_request_command(int message_type, bool enable);
	void send_set_threshold_command(const std::string &threshold_list, bool overwrite);
	void send_set_log_watch_command(bool enable, int log_level);
	void send_request_debug_categories_command();
	void send_request_topology_command();
	void send_enum_type_request_command(const std::string &enum_name);
	void send_property_command(const std::string &path, const std::string &property_name, GValue *gvalue);
	void send_request_factory(const std::string &factory_name);
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_COMMAND_FACTORY_H_ */
