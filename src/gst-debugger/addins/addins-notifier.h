/*
 *  addins-notifier.h
 *
 *  Created on: Oct 14, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_ADDINS_ADDINS_NOTIFIER_H_
#define SRC_GST_DEBUGGER_ADDINS_ADDINS_NOTIFIER_H_

#include "gstdebugger.pb.h"

#include <sigc++/sigc++.h>

namespace GstDebugger {

class AddinsNotifier
{
public:
	virtual ~AddinsNotifier() {}

	sigc::signal<void, const GstDebugger::QueryInfo&> on_query_received;
	sigc::signal<void, const GstDebugger::BufferInfo&> on_buffer_received;
	sigc::signal<void, const GstDebugger::EventInfo&> on_event_received;
    sigc::signal<void, const GstDebugger::MessageInfo&> on_message_received;
    sigc::signal<void, const GstDebugger::LogInfo&> on_log_received;
    sigc::signal<void, const GstDebugger::ServerError&> on_error_received;
};

}

#endif /* SRC_GST_DEBUGGER_ADDINS_ADDINS_NOTIFIER_H_ */
