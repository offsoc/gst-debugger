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
	void append_qe_entry(GstreamerQEBM *qebm) override;

	void display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe, const std::string &pad_path) override;

public:
	GstEventModule(const Glib::RefPtr<Gtk::Builder>& builder);

};

#endif /* SRC_GST_DEBUGGER_GST_EVENT_MODULE_H_ */
