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
#include "controller/element_path_processor.h"
#include "ui_utils.h"

#include <gst/gst.h>

static void free_properties(Property *property) { delete property; }

GstPropertiesModule::GstPropertiesModule(const Glib::RefPtr<Gtk::Builder>& builder)
{
	builder->get_widget("showPropertiesButton", show_propetries_button);
	show_propetries_button->signal_clicked().connect(sigc::mem_fun(*this, &GstPropertiesModule::showPropertiesButton_clicked_cb));

	builder->get_widget("propertiesBox", properties_box);

	create_dispatcher("property", sigc::mem_fun(*this, &GstPropertiesModule::new_property_), (GDestroyNotify)free_properties);
	create_dispatcher("selected-object-changed", sigc::mem_fun(*this, &GstPropertiesModule::selected_object_changed_), nullptr);
}

void GstPropertiesModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_property_received.connect(sigc::mem_fun(*this, &GstPropertiesModule::new_property));
	controller->on_selected_object_changed.connect(sigc::mem_fun(*this, &GstPropertiesModule::selected_object_changed));
}

void GstPropertiesModule::new_property(const Property &property)
{
	gui_push("property", new Property (property));
	gui_emit("property");
}

void GstPropertiesModule::request_selected_element_property(const std::string &property_name)
{
	auto obj = controller->get_selected_object();
	if (!obj || !std::dynamic_pointer_cast<ElementModel>(obj))
	{
		return;
	}

	std::string element_path = ElementPathProcessor::get_object_path(obj);
	controller->send_property_request_command(element_path, property_name);

	if (element_path != previous_element_path)
	{
		clear_widgets();
		previous_element_path = element_path;
	}
}

void GstPropertiesModule::showPropertiesButton_clicked_cb()
{
	request_selected_element_property("");
}

void GstPropertiesModule::new_property_()
{
	auto property = gui_pop<Property*>("property");

	auto element = std::dynamic_pointer_cast<ElementModel>(ElementPathProcessor(property->element_path()).get_last_obj());
	if (!element)
	{
		return;
	}

	std::shared_ptr<GValueBase> value_base = element->get_property(property->property_name());
	std::shared_ptr<GValueEnum> value_enum = std::dynamic_pointer_cast<GValueEnum>(value_base);

	auto& container = const_cast<RemoteDataContainer<GstEnumType>&>(controller->get_enum_container());
	if (value_enum && container.has_item(property->type_name()))
	{
		value_enum->set_type(container.get_item(property->type_name()));
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

		Gtk::Label *label = nullptr;
		Gtk::Widget *widget = nullptr;

		for (auto cd : hb->get_children())
		{
			if (label == nullptr && (label = dynamic_cast<Gtk::Label*>(cd)) != nullptr)
			{
				if (label->get_text() != property->property_name())
				{
					label = nullptr;
					break;
				}
			}
			else if (dynamic_cast<Gtk::Button*>(cd) == nullptr || dynamic_cast<Gtk::CheckButton*>(cd) != nullptr)
			{
				widget = cd;
			}
		}

		if (label == nullptr || widget == nullptr)
		{
			continue;
		}

		hb->remove(*widget);
		widget = value_base->get_widget();
		widget->show();
		hb->pack_start(*widget, true, 10);
		hb->reorder_child(*widget, 1);
		return true;
	}

	return false;
}

void GstPropertiesModule::append_property(const std::shared_ptr<GValueBase>& value_base, Property *property)
{
	Gtk::Box *hbox = new Gtk::Box (Gtk::ORIENTATION_HORIZONTAL, 0);
	hbox->show();
	auto prop_name = property->property_name();
	Gtk::Label *lbl = Gtk::manage(new Gtk::Label(prop_name));
	lbl->set_tooltip_text(property->description());
	lbl->show();
	Gtk::Button *btn = Gtk::manage(new Gtk::Button("Refresh"));
	btn->signal_clicked().connect([this, prop_name] {request_selected_element_property(prop_name);});
	btn->show();
	hbox->pack_start(*lbl, false, false);
	auto value_widget = value_base->get_widget();
	value_widget->show();
	hbox->pack_start(*value_widget, true, true);
	hbox->pack_start(*btn, false, false);
	properties_box->pack_start(*hbox);
}

void GstPropertiesModule::clear_widgets()
{
	for (auto c : properties_box->get_children())
	{
		delete c;
	}
}

void GstPropertiesModule::selected_object_changed()
{
	gui_emit("selected-object-changed");
}

void GstPropertiesModule::selected_object_changed_()
{
	clear_widgets();

	auto element = std::dynamic_pointer_cast<ElementModel>(controller->get_selected_object());

	if (element)
	{
		request_selected_element_property("");
		return;
	}

	auto pad = std::dynamic_pointer_cast<PadModel>(controller->get_selected_object());

	if (pad)
	{
		show_pad_properties();
	}
}

class PadPropertyModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<Glib::ustring> m_col_value;

	PadPropertyModelColumns()
	{
		add(m_col_name);
		add(m_col_value);
	}
};

void GstPropertiesModule::show_pad_properties()
{
	auto pad = std::dynamic_pointer_cast<PadModel>(controller->get_selected_object());

	if (!pad)
	{
		return;
	}

	PadPropertyModelColumns cols;
	auto model = Gtk::TreeStore::create(cols);
	Gtk::TreeView *tree = Gtk::manage(new Gtk::TreeView());
	tree->append_column("Property Name", cols.m_col_name);
	tree->append_column("Property Value", cols.m_col_value);
	tree->set_model(model);

#define APPEND_ROW(name, value) \
	do { \
		row = *(model->append()); \
		row[cols.m_col_name] = name; \
		row[cols.m_col_value] = value; \
	} while (false);

	std::string peer_pad = pad->get_peer() ? ElementPathProcessor::get_object_path(pad->get_peer()) : std::string("NO PEER PAD");

	Gtk::TreeModel::Row row;
	APPEND_ROW("Name", pad->get_name());

	if (pad->get_template())
	{
		display_template_info(pad->get_template(), model, cols.m_col_name, cols.m_col_value);
	}

	APPEND_ROW("Presence", get_presence_str(pad->get_presence()));
	APPEND_ROW("Direction", get_direction_str(pad->get_direction()));
	APPEND_ROW("Peer pad", peer_pad);
	APPEND_ROW("Current caps", pad->get_current_caps() ? pad->get_current_caps()->to_string() : Glib::ustring("unknown"));
	APPEND_ROW("Allowed caps", pad->get_allowed_caps() ? pad->get_allowed_caps()->to_string() : Glib::ustring("unknown"));

#undef APPEND_ROW

	tree->show();
	properties_box->pack_start(*tree, true, true, 0);
}
