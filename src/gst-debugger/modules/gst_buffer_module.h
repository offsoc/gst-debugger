/*
 * gst_buffer_module.h
 *
 *  Created on: Jul 5, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GST_BUFFER_MODULE_H_
#define SRC_GST_DEBUGGER_GST_BUFFER_MODULE_H_

#include "gst_qe_module.h"
#include "dialogs/buffer_data_dialog.h"

class GstBufferModule : public GstQEModule
{
	Glib::RefPtr<Gst::Buffer> buffer;
	BufferDataDialog *data_dialog;

	void append_qe_entry(GstreamerQEBM *qebm) override;

	void display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe, const std::string &pad_path) override;

	void qeDetialsTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);

public:
	GstBufferModule(const Glib::RefPtr<Gtk::Builder>& builder);

};

#endif /* SRC_GST_DEBUGGER_GST_BUFFER_MODULE_H_ */
