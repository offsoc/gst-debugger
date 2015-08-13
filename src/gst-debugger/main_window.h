/*
 * main_window.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_MAIN_WINDOW_H_
#define SRC_GST_DEBUGGER_MAIN_WINDOW_H_

#include "controller/tcp_client.h"
#include "dialogs/connection_properties_dialog.h"
#include "dialogs/enums_dialog.h"
#include "dialogs/factories_dialog.h"
#include "modules/gst_log_module.h"
#include "modules/gst_event_module.h"
#include "modules/gst_query_module.h"
#include "modules/gst_message_module.h"
#include "modules/gst_buffer_module.h"
#include "modules/gst_properties_module.h"
#include "pipeline-drawer/graph_module.h"

#include <gtkmm.h>

#include "controller/iview.h"
#include "models/gst_enum_model.h"

class MainWindow : public IMainView
{
	void connectionPropertiesMenuItem_activate_cb();
	void connectMenuItem_activate_cb();
	void connection_status_changed(bool connected);

	Glib::RefPtr<Gtk::Builder> get_dialog_ui_def();

	template<typename T>
	T* load_dialog(const std::string &name)
	{
		T* tmp = nullptr;
		Glib::RefPtr<Gtk::Builder> dialogs_builder = get_dialog_ui_def();
		dialogs_builder->get_widget_derived(name, tmp);
		return tmp;
	}

	Glib::RefPtr<Gtk::Builder> builder;
	Gtk::MenuItem *connection_properties;
	Gtk::MenuItem *remote_enum_types;
	Gtk::MenuItem *remote_factories;
	Gtk::ImageMenuItem *connect_menu_item;
	ConnectionPropertiesDialog *connection_properties_dialog;
	EnumsDialog *enums_dialog;
	FactoriesDialog *factories_dialog;
	Gtk::Statusbar *main_statusbar;

	std::shared_ptr<Glib::Dispatcher> dispatcher;
	std::shared_ptr<GstLogModule> log_module;
	std::shared_ptr<GstEventModule> event_module;
	std::shared_ptr<GstQueryModule> query_module;
	std::shared_ptr<GstMessageModule> message_module;
	std::shared_ptr<GstBufferModule> buffer_module;
	std::shared_ptr<GraphModule> graph_module;
	std::shared_ptr<GstPropertiesModule> properties_module;

	GstreamerInfo info;

public:
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
	virtual ~MainWindow() {}

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_MAIN_WINDOW_H_ */
