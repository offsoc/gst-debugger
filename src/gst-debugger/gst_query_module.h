/*
 * gst_query_module.h
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_QUERY_MODULE_H_
#define SRC_GST_QUERY_MODULE_H_

#include "gst_qe_module.h"

class GstQueryModule : public GstQEModule
{
	void append_qe_entry() override;

	void display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe) override;

public:
	GstQueryModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<TcpClient>& client);

};

#endif /* SRC_GST_QUERY_MODULE_H_ */
