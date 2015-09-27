/*
 * topology_controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_TOPOLOGY_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_TOPOLOGY_CONTROLLER_H_

#include "base_controller.h"

#include "common/gstdebugger.pb.h"

#include <mutex>

class TopologyController : public virtual BaseController
{
	std::mutex m_topology;

	void process_element(const GstDebugger::TopologyElement &element, GstDebugger::Action action);
	void process_pad(const GstDebugger::TopologyPad &pad, GstDebugger::Action action);
	void process_link(const GstDebugger::TopologyLink &link, GstDebugger::Action action);

public:
	void lock_topology() { m_topology.lock(); }
	void unlock_topology() { m_topology.unlock(); }
	void process(const GstDebugger::TopologyInfo &topology);
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_TOPOLOGY_CONTROLLER_H_ */
