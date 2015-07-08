/*
 * frame_receiver.h
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_FRAME_RECEIVER_H_
#define SRC_GST_DEBUGGER_FRAME_RECEIVER_H_

#include "protocol/gstdebugger.pb.h"

#include <glibmm.h>

#include <mutex>

class FrameReceiver
{
	std::mutex m;

	void process_frame_p()
	{
		try
		{
			process_frame();
		}
		catch (...)
		{
			// todo notify about it
		}
		m.unlock();
	}

protected:
	Glib::Dispatcher dispatcher;
	GstreamerInfo info;

	virtual void process_frame() = 0;

public:
	FrameReceiver()
	{
		dispatcher.connect(sigc::mem_fun(*this, &FrameReceiver::process_frame_p));
	}
	virtual ~FrameReceiver() {}

	void on_frame_recieved(const GstreamerInfo& info)
	{
		m.lock();
		this->info = info;
		dispatcher.emit();
	}
};



#endif /* SRC_GST_DEBUGGER_FRAME_RECEIVER_H_ */
