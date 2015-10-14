/*
 * controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_

#include "iview.h"
#include "connection_controller.h"
#include "command_factory.h"
#include "topology_controller.h"
#include "addins_controller.h"

#include "models/gst_enum_model.h"
#include "models/gst_factory_model.h"
#include "models/gst_klass_model.h"

#include <boost/optional/optional.hpp>

class Controller :
		public std::enable_shared_from_this<Controller>,
		public ConnectionController,
		public CommandFactory,
		public TopologyController,
		public AddinsController
{
private:
	IMainView *view;

	std::vector<GstEnumType> enum_container;
	std::vector<FactoryModel> factory_container;
	std::vector<KlassModel> klass_container;
	std::vector<std::string> debug_categories;

	std::shared_ptr<ObjectModel> selected_object;


	void process_frame(const GstDebugger::GStreamerData& info);

	void update_enum_model(const GstDebugger::EnumFlagsType &enum_type);
	void update_factory_model(const GstDebugger::FactoryType &factory_info);
	void update_klass_model(const GstDebugger::ElementKlass &klass_element);
	void update_pad_dynamic_info(const GstDebugger::PadDynamicInfo& info);
	void add_property(const GstDebugger::PropertyValue &value);

	void client_disconnected();

public:
	Controller(IMainView *view);

	int run(int &argc, char **&argv);

	void model_up();
	void model_down(const std::string &name);

	void set_selected_object(const std::string &name);

	const std::vector<std::string>& get_debug_categories() const { return debug_categories; }

	boost::optional<GstEnumType> get_enum_type(const std::string &name);
	boost::optional<FactoryModel> get_factory(const std::string &name);
	boost::optional<KlassModel> get_klass(const std::string &name);

	const std::vector<FactoryModel>& get_factories() const { return factory_container; }
	const std::vector<GstEnumType>& get_enums() const { return enum_container; }
	const std::vector<KlassModel>& get_klasses() const { return klass_container; }

	std::shared_ptr<ObjectModel> get_selected_object() const { return selected_object; }

	std::string get_selected_pad_path() const;

	void log(const std::string &message);

	sigc::signal<void> on_debug_categories_changed;
	sigc::signal<void, const GstDebugger::Command&> on_confirmation_received;
	sigc::signal<void, const Glib::ustring&, bool> on_enum_list_changed; /* enum name, true - add, false - remove */
	sigc::signal<void, const Glib::ustring&, bool> on_factory_list_changed;
	sigc::signal<void, const Glib::ustring&, bool> on_klass_list_changed;
	sigc::signal<void, std::shared_ptr<ElementModel>> on_model_changed;
	sigc::signal<void, const GstDebugger::PropertyValue&> on_property_value_received;
	sigc::signal<void> on_selected_object_changed;
	sigc::signal<void, const GstDebugger::GStreamerData&> on_frame_received;
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_ */
