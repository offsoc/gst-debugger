/*
 * gst_buffer_module.h
 *
 *  Created on: Jul 5, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_BUFFER_MODULE_H_
#define SRC_GST_DEBUGGER_GST_BUFFER_MODULE_H_

#include "gst_qe_module.h"

class GstBufferModule : public GstQEModule
{
	void append_qe_entry() override;

	void display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe) override;

public:
	GstBufferModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client);

};

#endif /* SRC_GST_DEBUGGER_GST_BUFFER_MODULE_H_ */
