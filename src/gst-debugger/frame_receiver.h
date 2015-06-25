/*
 * frame_receiver.h
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_FRAME_RECEIVER_H_
#define SRC_GST_DEBUGGER_FRAME_RECEIVER_H_

#include "protocol/gstdebugger.pb.h"

class FrameReceiver
{
protected:
	Glib::Dispatcher dispatcher;
	GstreamerInfo info;

	virtual void process_frame() = 0;

public:
	FrameReceiver()
	{
		dispatcher.connect(sigc::mem_fun(*this, &FrameReceiver::process_frame));
	}
	virtual ~FrameReceiver() {}

	void on_frame_recieved(const GstreamerInfo& info)
	{
		this->info = info;
		dispatcher.emit();
	}
};



#endif /* SRC_GST_DEBUGGER_FRAME_RECEIVER_H_ */
