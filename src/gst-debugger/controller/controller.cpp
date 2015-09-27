/*
 * controller.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "controller.h"
#include "ui_utils.h"

#include <gtkmm.h>

Controller::Controller(IMainView *view)
 : view(view)
{
	client->signal_frame_received.connect(sigc::mem_fun(*this, &Controller::process_frame));
	client->signal_status_changed.connect([this](bool connected) {
		if (!connected) client_disconnected();
		else
		{
			send_data_type_request_command("GstMessageType", GstDebugger::TypeDescriptionRequest_Type_ENUM_FLAGS);
			send_data_type_request_command("GstEventType", GstDebugger::TypeDescriptionRequest_Type_ENUM_FLAGS);
			send_data_type_request_command("GstQueryType", GstDebugger::TypeDescriptionRequest_Type_ENUM_FLAGS);
			send_request_entire_topology_command();
			send_request_debug_categories_command();
		}
	});
}

int Controller::run(int &argc, char **&argv)
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "eu.cookandcommit.gst-debugger");
	view->set_controller(shared_from_this());
	return app->run(*view);
}

void Controller::process_frame(const GstDebugger::GStreamerData &data)
{
	on_frame_received(data);

	switch (data.info_type_case())
	{
	case GstDebugger::GStreamerData::kDebugCategories:
		debug_categories.clear();
		for (auto c : data.debug_categories().category())
			debug_categories.push_back(c);
		on_debug_categories_changed();
		break;
	case GstDebugger::GStreamerData::kEnumFlagsType:
		update_enum_model(data.enum_flags_type());
		on_enum_list_changed(data.enum_flags_type().type_name(), true);
		break;
	case GstDebugger::GStreamerData::kConfirmation:
		on_confirmation_received(data.confirmation());
		break;
	case GstDebugger::GStreamerData::kTopologyInfo:
		process(data.topology_info());
		on_model_changed(current_model);

		if (data.topology_info().has_element() && !get_factory(data.topology_info().element().factory_name()))
		{
			send_request_factory_command(data.topology_info().element().factory_name());
		}
		break;
	case GstDebugger::GStreamerData::kFactory:
		update_factory_model(data.factory());
		on_factory_list_changed(data.factory().name(), true);
		break;
	}
	/*
	case GstreamerInfo_InfoType_PROPERTY:
	{
		std::string name = info.property().type_name();
		if ((info.property().internal_type() == INTERNAL_GTYPE_ENUM || info.property().internal_type() == INTERNAL_GTYPE_FLAGS) && !enum_container.has_item(name))
		{
			send_enum_type_request_command(name);
		}
		append_property(info.property());
		on_property_received(info.property());
		break;
	}
	case GstreamerInfo_InfoType_FACTORY:
		update_factory_model(info.factory_info());
		on_factory_list_changed(info.factory_info().name());
		break;
	case GstreamerInfo_InfoType_PAD_DYNAMIC_INFO:
		update_pad_dynamic_info(info.pad_dynamic_info());
		break;
	default:
		break;
	}*/
}

template<typename T>
boost::optional<T> get_from_container(const std::vector<T>& container, const std::string &name, std::function<std::string(const T& val)> get_t_name)
{
	auto it = std::find_if(container.begin(), container.end(), [name, get_t_name](const T& enum_type) {
		return get_t_name(enum_type) == name;
	});

	if (it == container.end())
		return boost::none;
	else
		return *it;
}

boost::optional<GstEnumType> Controller::get_enum_type(const std::string &name)
{
	return get_from_container<GstEnumType>(enum_container, name, [](const GstEnumType& enum_type) {return enum_type.get_name(); } );
}

boost::optional<FactoryModel> Controller::get_factory(const std::string &name)
{
	return get_from_container<FactoryModel>(factory_container, name, [](const FactoryModel& factory) {return factory.get_name(); } );
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
	bool is_pad = colon_pos != std::string::npos;
	std::shared_ptr<ObjectModel> obj;

	if (!is_pad)
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

		if (obj && is_pad)
		{
		//	send_request_pad_dynamic_info(ElementPathProcessor::get_object_path(obj));
		}
	}
}

void Controller::update_enum_model(const GstDebugger::EnumFlagsType &enum_type)
{
	GstEnumType et(enum_type.type_name(), G_TYPE_ENUM); // todo G_TYPE_FLAGS
	for (int i = 0; i < enum_type.values_size(); i++)
	{
		et.add_value(enum_type.values(i).name(), enum_type.values(i).value(), enum_type.values(i).nick());
	}

	// todo copy & paste get_enum_type()
	auto it = std::find_if(enum_container.begin(), enum_container.end(), [enum_type](const GstEnumType& type) {
		return type.get_name() == enum_type.type_name();
	});

	if (it == enum_container.end())
	{
		enum_container.push_back(et);
	}
	else
	{
		*it = et;
	}
}

void Controller::update_factory_model(const GstDebugger::FactoryType &factory_info)
{
	FactoryModel model(factory_info.name());

	for (int i = 0; i < factory_info.templates_size(); i++)
	{
		model.append_template(protocol_template_to_gst_template(factory_info.templates(i)));
	}

	for (int i = 0; i < factory_info.metadata_size(); i++)
	{
		model.append_meta(factory_info.metadata(i).key(), factory_info.metadata(i).value());
	}

	// todo copy & paste get_enum_type()
	auto it = std::find_if(factory_container.begin(), factory_container.end(), [model](const FactoryModel& type) {
		return type.get_name() == model.get_name();
	});

	if (it == factory_container.end())
	{
		factory_container.push_back(model);
	}
	else
	{
		*it = model;
	}
}
/*
void Controller::append_property(const Property& property)
{
	GValue *value = new GValue;
	*value = {0};
	g_value_deserialize(value, property.type(), (InternalGType)property.internal_type(), property.property_value().c_str());

	auto element = ElementModel::get_parent_element_from_path(property.element_path());
	std::shared_ptr<GValueBase> gvalue(GValueBase::build_gvalue(value));
	element->add_property(property.property_name(), gvalue);
	gvalue->widget_value_changed.connect([gvalue, property, this] {
		this->send_property_command(property.element_path(), property.property_name(), gvalue->get_gvalue());
	});
}
*/
void Controller::log(const std::string &message)
{
	// todo date/time?
	std::cerr << message << std::endl;

	//on_new_log_entry(message);
}

void Controller::client_disconnected()
{

/*	auto root_model = ElementModel::get_root();
	root_model->clean_model();
	on_model_changed(root_model);

	selected_object = std::shared_ptr<ObjectModel>();
	on_selected_object_changed();

	while (enum_container.begin() != enum_container.end())
	{
		std::string name = enum_container.begin()->get_name();
		enum_container.remove_item(name);
		on_enum_list_changed(name, false);
	}
	while (factory_container.begin() != factory_container.end())
	{
		std::string name = factory_container.begin()->get_name();
		factory_container.remove_item(name);
		on_factory_list_changed(name);
	}

	debug_categories.clear();
	on_debug_categories_changed();*/
}
/*
void Controller::update_pad_dynamic_info(const PadDynamicInfo &info)
{
	auto pad = std::dynamic_pointer_cast<PadModel>(ElementPathProcessor(info.pad_path()).get_last_obj());

	if (!pad)
		return;

	pad->set_current_caps(Gst::Caps::create_from_string(info.current_caps()));
	pad->set_allowed_caps(Gst::Caps::create_from_string(info.allowed_caps()));

	if (selected_object == pad)
	{
		on_selected_object_changed();
	}
}*/

