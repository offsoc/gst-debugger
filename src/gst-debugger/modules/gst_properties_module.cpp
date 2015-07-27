/*
 * gst_properties_module.cpp
 *
 *  Created on: Jul 14, 2015
 *      Author: loganek
 */

#include "gst_properties_module.h"
#include "gvalue-converter/gvalue_enum.h"
#include "protocol/deserializer.h"
#include "controller/command_factory.h"
#include "controller/controller.h"
#include <gst/gst.h>

static void free_properties(Property *property) { delete property; }

GstPropertiesModule::GstPropertiesModule(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("showPropertiesButton", show_propetries_button);
	show_propetries_button->signal_clicked().connect(sigc::mem_fun(*this, &GstPropertiesModule::showPropertiesButton_clicked_cb));

	builder->get_widget("propertiesBox", properties_box);

	builder->get_widget("elementPathPropertyEntry", element_path_property_entry);

	create_dispatcher("property", sigc::mem_fun(*this, &GstPropertiesModule::new_property_), (GDestroyNotify)free_properties);
}

void GstPropertiesModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_property_received.connect(sigc::mem_fun(*this, &GstPropertiesModule::new_property));
}

void GstPropertiesModule::new_property(const Property &property)
{
	gui_push("property", new Property (property));
	gui_emit("property");
}

void GstPropertiesModule::request_property(const std::string &property_name)
{
	controller->send_property_request_command(element_path_property_entry->get_text(), property_name);
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

void GstPropertiesModule::new_property_()
{
	auto property = gui_pop<Property*>("property");
	GValue value = {0};
	g_value_deserialize(&value, property->type(), (InternalGType)property->internal_type(), property->property_value().c_str());

	std::shared_ptr<GValueBase> value_base(GValueBase::build_gvalue(&value));
	std::shared_ptr<GValueEnum> value_enum = std::dynamic_pointer_cast<GValueEnum>(value_base);

	if (value_enum && const_cast<GstEnumContainer&>(controller->get_enum_container()).has_type(property->type_name()))
	{
		value_enum->set_type(const_cast<GstEnumContainer&>(controller->get_enum_container()).get_type(property->type_name()));
	}

	if (!update_property(value_base, property))
	{
		append_property(value_base, property);
	}
	delete property;
}

bool GstPropertiesModule::update_property(const std::shared_ptr<GValueBase>& value_base, Property *property)
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

		if (label->get_text() == property->property_name())
		{
			entry->set_text(value_base->to_string());
			return true;
		}
	}

	return false;
}

void GstPropertiesModule::append_property(const std::shared_ptr<GValueBase>& value_base, Property *property)
{
	Gtk::Box *hbox = new Gtk::Box (Gtk::ORIENTATION_HORIZONTAL, 0);
	hbox->show();
	auto prop_name = property->property_name();
	Gtk::Label *lbl = Gtk::manage(new Gtk::Label(prop_name));
	lbl->show();
	Gtk::Button *btn = Gtk::manage(new Gtk::Button("Refresh"));
	btn->signal_clicked().connect([this, prop_name] {request_property(prop_name);});
	btn->show();
	hbox->pack_start(*lbl, false, false);
	auto value_widget = value_base->get_widget();
	value_widget->show();
	hbox->pack_start(*value_widget, true, true);
	hbox->pack_start(*btn, false, false);
	properties_box->pack_start(*hbox);
	property_widgets.push_back(hbox);
}
