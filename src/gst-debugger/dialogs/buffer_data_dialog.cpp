/*
 * buffer_data_dialog.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: loganek
 */

#include "buffer_data_dialog.h"

StringDataFormat BufferDataDialog::get_selected_format() const
{
	for (gsize i = 0; i < format_buttons.size(); i++)
	{
		if (format_buttons[i]->get_active())
		{
			return static_cast<StringDataFormat>(i);
		}
	}
	return StringDataFormat::HEX;
}

BufferDataDialog::BufferDataDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Dialog(cobject)
{
	builder->get_widget("bufferDataCloseButton", close_button);
	close_button->signal_clicked().connect([this]{hide();});

	text_buffer = Gtk::TextBuffer::create();
	builder->get_widget("bufferDataTextView", data_text_view);
	data_text_view->set_buffer(text_buffer);

	int i = 0;
	for (auto format_name : {"bin", "oct", "dec", "hex"})
	{
		builder->get_widget(format_name + std::string("FormatRadioButton"), format_buttons[i]);
		format_buttons[i]->signal_toggled().connect([this, i]{change_format(i);});
		i++;
	}

	builder->get_widget("columnsInRowEntry", columns_in_row_entry);
	builder->get_widget("setColumnsInRowButton", set_columns_in_row_button);
	set_columns_in_row_button->signal_clicked().connect([this] {
		cols_in_row = std::max(1, std::atoi(columns_in_row_entry->get_text().c_str()));
		set_buffer(buf);
	});
}

void BufferDataDialog::change_format(int i)
{
	if (format_buttons[i]->get_active())
	{
		set_buffer(buf);
	}
}

void BufferDataDialog::set_buffer(const Glib::RefPtr<Gst::Buffer> &buffer)
{
	buf = buffer;
	text_buffer->set_text(buffer_data_to_string(get_selected_format(), buf, buf->get_size(), cols_in_row));
}

