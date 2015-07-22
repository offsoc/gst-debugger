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

class TopologyController : public virtual BaseController
{
public:
	void process(const Topology &topology);
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_TOPOLOGY_CONTROLLER_H_ */
