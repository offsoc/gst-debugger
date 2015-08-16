/*
 * buffer_data_dialog.h
 *
 *  Created on: Aug 16, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_DIALOGS_BUFFER_DATA_DIALOG_H_
#define SRC_GST_DEBUGGER_DIALOGS_BUFFER_DATA_DIALOG_H_

#include "ui_utils.h"

#include <gstreamermm.h>
#include <gtkmm.h>

#include <array>

class BufferDataDialog : public Gtk::Dialog
{
	Gtk::Button *close_button;
	Gtk::TextView *data_text_view;
	std::array<Gtk::RadioButton*, 4> format_buttons; // in order: bin, oct, dec, hex
	Gtk::Entry *columns_in_row_entry;
	Gtk::Button *set_columns_in_row_button;

	Glib::RefPtr<Gst::Buffer> buf;
	Glib::RefPtr<Gtk::TextBuffer> text_buffer;

	int cols_in_row = 16;

	StringDataFormat get_selected_format() const;

public:
	BufferDataDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

	void change_format(int i);

	void set_buffer(const Glib::RefPtr<Gst::Buffer> &buffer);
};

#endif /* SRC_GST_DEBUGGER_DIALOGS_BUFFER_DATA_DIALOG_H_ */
