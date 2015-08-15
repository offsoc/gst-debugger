/*
 * topology_controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_TOPOLOGY_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_TOPOLOGY_CONTROLLER_H_

#include "base_controller.h"

#include "protocol/gstdebugger.pb.h"

#include <mutex>

class TopologyController : public virtual BaseController
{
	std::mutex m_topology;

	void process_element(const TopologyElement &element, Topology_Action action);
	void process_pad(const TopologyPad &pad, Topology_Action action);
	void process_link(const TopologyLink &link, Topology_Action action);

public:
	void lock_topology() { m_topology.lock(); }
	void unlock_topology() { m_topology.unlock(); }
	void process(const Topology &topology);
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_TOPOLOGY_CONTROLLER_H_ */
