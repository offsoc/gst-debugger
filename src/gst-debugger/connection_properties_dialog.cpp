/*
 * connection_properties_dialog.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "connection_properties_dialog.h"

#include <giomm.h>

#include <string>

ConnectionPropertiesDialog::ConnectionPropertiesDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Dialog(cobject)
{
	builder->get_widget("pingButton", ping_button);
	ping_button->signal_clicked().connect(sigc::mem_fun(*this, &ConnectionPropertiesDialog::pingButton_click_cb));

	builder->get_widget("ipAddressEntry", ip_address_entry);
	builder->get_widget("portEntry", port_entry);

	builder->get_widget("closeButton", close_button);
	close_button->signal_clicked().connect([this]{hide();});
}

void ConnectionPropertiesDialog::pingButton_click_cb()
{
	auto client = Gio::SocketClient::create();
	try
	{
		auto connection = client->connect_to_host(get_ip_address(), get_port());
		if (connection->is_connected())
		{
			Gtk::MessageDialog dialog("Info");
			dialog.set_secondary_text("PING OK!");
			dialog.run();
			connection->close();
		}
		else
			throw Gio::Error(Gio::Error::FAILED, "");
	}
	catch(const Gio::Error&)
	{
		Gtk::MessageDialog dialog("Info", false, Gtk::MESSAGE_ERROR);
		dialog.set_secondary_text("Server doesn't response");
		dialog.run();
	}
}

int ConnectionPropertiesDialog::get_port() const
{
	return std::stoi(port_entry->get_text());
}

std::string ConnectionPropertiesDialog::get_ip_address() const
{
	return ip_address_entry->get_text();
}
