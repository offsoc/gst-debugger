/*
 * controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_

#include "command_factory.h"
#include "iview.h"
#include "connection_controller.h"
#include "topology_controller.h"
#include "models/gst_enum_model.h"
#include "models/remote_data_container.h"
#include "models/gst_factory_model.h"
#include "protocol/gstdebugger.pb.h"

class Controller :
		public std::enable_shared_from_this<Controller>,
		public CommandFactory,
		public ConnectionController,
		public TopologyController
{
private:
	IMainView *view;

	std::shared_ptr<ObjectModel> selected_object;

	RemoteDataContainer<GstEnumType> enum_container;
	RemoteDataContainer<FactoryModel> factory_container;
	std::vector<std::string> debug_categories;


	void process_frame(const GstreamerInfo& info);

	void update_enum_model(const EnumType &enum_type);

	void update_factory_model(const FactoryInfo &factory_info);

	void append_property(const Property& property);

	void client_disconnected();

	void update_pad_dynamic_info(const PadDynamicInfo& info);

public:
	Controller(IMainView *view);

	void send_command(const Command& cmd);

	int run(int &argc, char **&argv);

	void model_up();
	void model_down(const std::string &name);

	void set_selected_object(const std::string &name);

	const RemoteDataContainer<GstEnumType>& get_enum_container() const { return enum_container; }
	const RemoteDataContainer<FactoryModel>& get_factory_container() const { return factory_container; }
	const std::vector<std::string>& get_debug_categories() const { return debug_categories; }

	std::shared_ptr<ObjectModel> get_selected_object() const { return selected_object; }

	void log(const std::string &message);

	sigc::signal<void, const GstreamerLog&> on_log_received;
	sigc::signal<void> on_debug_categories_changed;
	sigc::signal<void, std::shared_ptr<ElementModel>> on_model_changed;
	sigc::signal<void, const Property&> on_property_received;
	sigc::signal<void, const GstreamerQEBM&, GstreamerInfo_InfoType> on_qebm_received;
	sigc::signal<void, const MessageWatch&> on_message_confirmation_received;
	sigc::signal<void, const PadWatch&, PadWatch_WatchType> on_pad_watch_confirmation_received;
	sigc::signal<void, const Glib::ustring&, bool> on_enum_list_changed; /* enum name, true - add, false - remove */
	sigc::signal<void> on_selected_object_changed;
	sigc::signal<void, const Glib::ustring&> on_factory_list_changed;
	sigc::signal<void, const Glib::ustring&> on_new_log_entry;
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_ */
