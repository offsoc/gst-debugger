/*
 * gst_properties_module.cpp
 *
 *  Created on: Jul 14, 2015
 *      Author: loganek
 */

#include "gst_properties_module.h"
#include "protocol/deserializer.h"

#include <gst/gst.h>

GstPropertiesModule::GstPropertiesModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client)
: client (client)
{
	builder->get_widget("showPropertiesButton", show_propetries_button);
	show_propetries_button->signal_clicked().connect(sigc::mem_fun(*this, &GstPropertiesModule::showPropertiesButton_clicked_cb));

	builder->get_widget("propertiesBox", properties_box);

	builder->get_widget("elementPathPropertyEntry", element_path_property_entry);
}

void GstPropertiesModule::request_property(const std::string &property_name)
{
	Command cmd;
	Property *property = new Property();
	property->set_element_path(element_path_property_entry->get_text());
	property->set_property_name(property_name);
	cmd.set_command_type(Command_CommandType_PROPERTY);
	cmd.set_allocated_property(property);

	client->send_command(cmd);
}

void GstPropertiesModule::showPropertiesButton_clicked_cb()
{
	std::string current_element_path = element_path_property_entry->get_text();
	request_property("");

	if (current_element_path != previous_element_path)
	{
		for (auto pw : property_widgets)
		{
			delete pw;
		}
		property_widgets.clear();
		previous_element_path = current_element_path;
	}
}

void GstPropertiesModule::process_frame()
{
	if (info.info_type() != GstreamerInfo_InfoType_PROPERTY)
		return;

	GValue value = {0};
	g_value_init (&value, info.property().type());
	gst_value_deserialize(&value, info.property().property_value().c_str());

	std::shared_ptr<GValueBase> value_base(GValueBase::build_gvalue(&value));

	if (!update_property(value_base))
	{
		append_property(value_base);
	}
}

bool GstPropertiesModule::update_property(const std::shared_ptr<GValueBase>& value_base)
{
	for (auto internal_box : properties_box->get_children())
	{
		Gtk::Box *hb = dynamic_cast<Gtk::Box*>(internal_box);
		if (hb == nullptr)
		{
			continue;
		}

		Gtk::Entry* entry = nullptr;
		Gtk::Label* label = nullptr;

		for (auto cd : hb->get_children())
		{
			if (entry == nullptr && (entry = dynamic_cast<Gtk::Entry*>(cd)) != nullptr);
			else if (label == nullptr)
			{
				label = dynamic_cast<Gtk::Label*>(cd);
			}
		}

		if (label == nullptr || entry == nullptr)
		{
			continue;
		}

		if (label->get_text() == info.property().property_name())
		{
			entry->set_text(value_base->to_string());
			return true;
		}
	}

	return false;
}

void GstPropertiesModule::append_property(const std::shared_ptr<GValueBase>& value_base)
{
	Gtk::Box *hbox = new Gtk::Box (Gtk::ORIENTATION_HORIZONTAL, 0);
	hbox->show();
	Gtk::Entry *entry = Gtk::manage(new Gtk::Entry());
	entry->set_text (value_base->to_string());
	entry->show();
	auto prop_name = info.property().property_name();
	Gtk::Label *lbl = Gtk::manage(new Gtk::Label(prop_name));
	lbl->show();
	Gtk::Button *btn = Gtk::manage(new Gtk::Button("Refresh"));
	btn->signal_clicked().connect([this, prop_name] {request_property(prop_name);});
	btn->show();
	hbox->pack_start(*lbl, false, false);
	hbox->pack_start(*entry, true, true);
	hbox->pack_start(*btn, false, false);
	properties_box->pack_start(*hbox);
	property_widgets.push_back(hbox);
}
