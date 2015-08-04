/*
 * controller.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "controller.h"

#include "protocol/common.h"
#include "protocol/deserializer.h"

#include <gtkmm.h>

Controller::Controller(IMainView *view)
 : view(view)
{
	client->signal_frame_received.connect(sigc::mem_fun(*this, &Controller::process_frame));
}

int Controller::run(int &argc, char **&argv)
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "gst.debugger.com");
	view->set_controller(shared_from_this());
	return app->run(*view);
}

void Controller::send_command(const Command& cmd)
{
	client->send_command(cmd);
}

void Controller::process_frame(const GstreamerInfo &info)
{
	switch (info.info_type())
	{
	case GstreamerInfo_InfoType_TOPOLOGY:
		process(info.topology());
		on_model_changed(current_model);
		break;
	case GstreamerInfo_InfoType_DEBUG_CATEGORIES:
		on_debug_categories_received(info.debug_categories());
		break;
	case GstreamerInfo_InfoType_LOG:
		on_log_received(info.log());
		break;
	case GstreamerInfo_InfoType_PROPERTY:
	{
		std::string name = info.property().type_name();
		if ((info.property().internal_type() == INTERNAL_GTYPE_ENUM || info.property().internal_type() == INTERNAL_GTYPE_FLAGS) && !enum_container.has_type(name))
		{
			send_enum_type_request_command(name);
		}
		append_property(info.property());
		on_property_received(info.property());
		break;
	}
	case GstreamerInfo_InfoType_BUFFER:
	case GstreamerInfo_InfoType_EVENT:
	case GstreamerInfo_InfoType_QUERY:
	case GstreamerInfo_InfoType_MESSAGE:
		on_qebm_received(info.qebm(), info.info_type());
		break;
	case GstreamerInfo_InfoType_PAD_WATCH_CONFIRMATION:
		on_pad_watch_confirmation_received(info.confirmation(), info.confirmation().watch_type());
		break;
	case GstreamerInfo_InfoType_MESSAGE_CONFIRMATION:
		on_message_confirmation_received(info.bus_msg_confirmation());
		break;
	case GstreamerInfo_InfoType_ENUM_TYPE:
		update_enum_model(info.enum_type());
		on_enum_list_changed();
		break;
	default:
		break;
	}
}

void Controller::model_up()
{
	if (current_model == ElementModel::get_root())
		return;

	current_model = std::static_pointer_cast<ElementModel>(current_model->get_parent());
	on_model_changed(current_model);
}

void Controller::model_down(const std::string &name)
{
	auto tmp = current_model->get_child(name);

	if (tmp && tmp->is_bin())
	{
		current_model = tmp;
		on_model_changed(current_model);
	}
}

void Controller::set_selected_object(const std::string &name)
{
	auto colon_pos = name.find(':');
	std::shared_ptr<ObjectModel> obj;

	if (colon_pos == std::string::npos)
	{
		obj = current_model->get_child(name);
	}
	else
	{
		std::string e_name = name.substr(0, colon_pos);
		std::string p_name = name.substr(colon_pos+1);
		std::shared_ptr<ElementModel> e_model;
		if ((e_model = std::dynamic_pointer_cast<ElementModel>(current_model->get_child(e_name))))
		{
			obj = e_model->get_pad(p_name);
		}
	}

	if (selected_object != obj)
	{
		selected_object = obj;
		on_selected_object_changed();
	}
}

void Controller::update_enum_model(const EnumType &enum_type)
{
	GstEnumType et(enum_type.type_name());
	for (int i = 0; i < enum_type.entry_size(); i++)
	{
		et.add_value(enum_type.entry(i).name(), enum_type.entry(i).value(), enum_type.entry(i).nick());
	}
	enum_container.update_type(et);
}

void Controller::append_property(const Property& property)
{
	GValue *value = new GValue;
	*value = {0};
	g_value_deserialize(value, property.type(), (InternalGType)property.internal_type(), property.property_value().c_str());

	auto element = ElementModel::get_parent_element_from_path(property.element_path());
	element->add_property(property.property_name(), std::shared_ptr<GValueBase>(GValueBase::build_gvalue(value)));
}
