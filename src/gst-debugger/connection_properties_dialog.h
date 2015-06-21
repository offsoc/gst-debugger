/*
 * connection_properties_dialog.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_CONNECTION_PROPERTIES_DIALOG_H_
#define SRC_GST_DEBUGGER_CONNECTION_PROPERTIES_DIALOG_H_

#include <gtkmm.h>

class ConnectionPropertiesDialog : public Gtk::Dialog
{
	Gtk::Button *ping_button;
	Gtk::Button *close_button;
	Gtk::Entry *ip_address_entry;
	Gtk::Entry *port_entry;

	void pingButton_click_cb();

public:
	ConnectionPropertiesDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

	int get_port() const;
	std::string get_ip_address() const;
};

#endif /* SRC_GST_DEBUGGER_CONNECTION_PROPERTIES_DIALOG_H_ */
