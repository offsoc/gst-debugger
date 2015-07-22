/*
 * command_factory.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_COMMAND_FACTORY_H_
#define SRC_GST_DEBUGGER_CONTROLLER_COMMAND_FACTORY_H_

#include "protocol/gstdebugger.pb.h"

#include "base_controller.h"

class CommandFactory : public virtual BaseController
{
public:
	static Command make_pad_watch_command(bool enable, PadWatch_WatchType watch_type, const std::string &pad_path, int qe_type);
	static Command make_property_request_command(const std::string &element_path, const std::string &property_name);
	static Command make_message_request_command(int message_type);
	static Command make_set_threshold_command(const std::string &threshold_list, bool overwrite);
	static Command make_set_log_watch_command(bool enable, int log_level);
	static Command make_request_debug_categories_command();
	static Command make_request_topology_command();
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_COMMAND_FACTORY_H_ */
