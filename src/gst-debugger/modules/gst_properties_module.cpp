/*
 * gst_properties_module.cpp
 *
 *  Created on: Jul 14, 2015
 *      Author: loganek
 */

#include "gst_properties_module.h"
#include "common/deserializer.h"
#include "controller/command_factory.h"
#include "controller/controller.h"
#include "controller/element_path_processor.h"
#include "ui_utils.h"

#include <gst/gst.h>

static void free_properties(GstDebugger::PropertyInfo *property) { delete property; }

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
	controller->on_property_value_received.connect(sigc::mem_fun(*this, &GstPropertiesModule::new_property));
	controller->on_selected_object_changed.connect(sigc::mem_fun(*this, &GstPropertiesModule::selected_object_changed));
}

void GstPropertiesModule::new_property(const GstDebugger::PropertyValue &property)
{
	gui_push("property", new GstDebugger::PropertyValue(property));
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
	auto property = gui_pop<GstDebugger::PropertyValue*>("property");

	auto element = std::dynamic_pointer_cast<ElementModel>(ElementPathProcessor(property->object()).get_last_obj());
	if (!element)
	{
		return;
	}

	std::shared_ptr<GValueBase> value_base = element->get_property(property->name());
	/*std::shared_ptr<GValueEnum> value_enum = std::dynamic_pointer_cast<GValueEnum>(value_base);

	if (value_enum && controller->get_enum_type(property->value().type_name()))
	{
		value_enum->set_type(container.get_item(property->value().type_name()));
	}*/

	if (!update_property(value_base, property->name()))
	{
		append_property(value_base, property->name());
	}

	delete property;
}

bool GstPropertiesModule::update_property(const std::shared_ptr<GValueBase>& value_base, const std::string prop_name)
{
	for (auto internal_box : properties_box->get_children())
	{
		Gtk::Box *hb = dynamic_cast<Gtk::Box*>(internal_box);
		if (hb == nullptr)
		{
			continue;
		}

		if (reinterpret_cast<gchar*>(hb->get_data("property-name")) != prop_name)
			continue;

		for (auto widget : hb->get_children())
		{
			if (widget->get_data("is-gvalue-widget") == GINT_TO_POINTER(1))
			{
				bool sensitive = widget->get_sensitive();
				hb->remove(*widget);
				widget = value_base->get_widget();
				widget->set_sensitive(sensitive);
				widget->show();
				hb->pack_start(*widget, true, 10);
				hb->reorder_child(*widget, 1);
				return true;
			}
		}
	}

	return false;
}

void GstPropertiesModule::append_property(const std::shared_ptr<GValueBase>& value_base, const std::string &prop_name)
{
	auto e = std::dynamic_pointer_cast<ElementModel>(controller->get_selected_object());
	if (!e) return;
	auto klass = controller->get_klass(e->get_type_name());
	if (!klass) return;
	auto prop = klass.get().get_property(prop_name);
	if (!prop) return;

	Gtk::Box *hbox = new Gtk::Box (Gtk::ORIENTATION_HORIZONTAL, 0);
	hbox->set_data("property-name", g_strdup (prop_name.c_str()), g_free);
	Gtk::Label *lbl = Gtk::manage(new Gtk::Label(prop_name));
	lbl->set_tooltip_text(prop->get_blurb());
	Gtk::Button *btn = Gtk::manage(new Gtk::Button("Refresh"));
	btn->signal_clicked().connect([this, prop_name] {request_selected_element_property(prop_name);});
	hbox->pack_start(*lbl, false, false);
	auto value_widget = value_base->get_widget();
	value_base->set_sensitive(prop.get().get_flags() & G_PARAM_WRITABLE);
	hbox->pack_start(*value_widget, true, true);
	hbox->pack_start(*btn, false, false);
	properties_box->pack_start(*hbox);
	hbox->show_all();
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

	if (pad->get_current_caps())
	{
		APPEND_ROW("Current caps", "");
		display_caps(pad->get_current_caps(), model, cols.m_col_name, cols.m_col_value, row);
	}
	else
	{
		APPEND_ROW("Current caps", "unknown");
	}

	if (pad->get_allowed_caps())
	{
		APPEND_ROW("Allowed caps", "");
		display_caps(pad->get_allowed_caps(), model, cols.m_col_name, cols.m_col_value, row);
	}
	else
	{
		APPEND_ROW("Allowed caps", "unknown");
	}

#undef APPEND_ROW

	tree->show();
	properties_box->pack_start(*tree, true, true, 0);
}
