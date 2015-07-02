/*
 * gst_event_module.h
 *
 *  Created on: Jun 25, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_EVENT_MODULE_H_
#define SRC_GST_DEBUGGER_GST_EVENT_MODULE_H_

#include "gst_qe_module.h"

class GstEventModule : public GstQEModule
{
	void append_qe_entry() override;

	void display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe) override;

public:
	GstEventModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client);

};

#endif /* SRC_GST_DEBUGGER_GST_EVENT_MODULE_H_ */
