/*
 * main_window.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mkolny
 */

#include "config.h"

#include "main_window.h"
#include "ui_utils.h"

#include "gst-debugger-resources.h"

#include "controller/controller.h"

#include <glib/gi18n.h>

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: IMainView(cobject),
  builder(builder),
  dispatcher(std::make_shared<Glib::Dispatcher>()),
  main_module(std::make_shared<MainModule>(builder)),
  graph_module(std::make_shared<GraphModule>(builder)),
  properties_module(std::make_shared<GstPropertiesModule>(builder))
{
	builder->get_widget("connectionPropertiesMenuItem", connection_properties);
	connection_properties->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::connectionPropertiesMenuItem_activate_cb));

	builder->get_widget("remoteEnumTypesMenuitem", remote_enum_types);
	remote_enum_types->signal_activate().connect([this] { enums_dialog->show(); });

	builder->get_widget("remoteFactoriesMenuitem", remote_factories);
	remote_factories->signal_activate().connect([this] { factories_dialog->show(); });

	builder->get_widget("remoteKlassesMenuitem", remote_klasses);
	remote_klasses->signal_activate().connect([this] { klasses_dialog->show(); });

	builder->get_widget("addinsMenuItem", addins_menu_item);

	Gtk::MenuItem *reload_addins_menu_item;
	builder->get_widget("reloadAddinsMenuItem", reload_addins_menu_item);
	reload_addins_menu_item->signal_activate().connect([this] { controller->reload_addins(); });

	builder->get_widget("connectMenuItem", connect_menu_item);
	connect_menu_item->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::connectMenuItem_activate_cb));

	builder->get_widget("aboutMenuItem", about_menu_item);
	about_menu_item->signal_activate().connect([this] { about_dialog->show(); });

	builder->get_widget("mainStatusbar", main_statusbar);

	builder->get_widget("quitMenuItem", quit_menu_item);
	quit_menu_item->signal_activate().connect([this] { this->close(); });

	enums_dialog = load_dialog<EnumsDialog>("remoteDataDialog");
	factories_dialog = load_dialog<FactoriesDialog>("remoteDataDialog");
	klasses_dialog = load_dialog<KlassesDialog>("remoteDataDialog");
	connection_properties_dialog = load_dialog<ConnectionPropertiesDialog>("connectionPropertiesDialog");

	{
		Glib::RefPtr<Gtk::Builder> dialogs_builder = get_dialog_ui_def();
		dialogs_builder->get_widget("aboutDialog", about_dialog);
		about_dialog->set_logo(Gdk::Pixbuf::create_from_resource("/eu/cookandcommit/gst-debugger/ui/gst-debugger-logo.png"));
		about_dialog->set_transient_for(*this);
		about_dialog->set_version(VERSION);
	}

	builder->get_widget("mainStatusbar", main_statusbar);

	connection_status_changed(false);

	signal_show().connect([this] {
		graph_module->update_model_();
	});

	signal_delete_event().connect([this](GdkEventAny*){
		graph_module->free_graph();
		return false;
	});
}

void MainWindow::set_controller(const std::shared_ptr<Controller> &controller)
{
	this->controller = controller;

	controller->on_connection_status_changed(sigc::mem_fun(*this, &MainWindow::connection_status_changed));
	controller->on_addins_reloaded.connect(sigc::mem_fun(*this, &MainWindow::addins_reloaded));

	controller->reload_addins();

	main_module->set_controller(controller);
	graph_module->set_controller(controller);

	enums_dialog->set_controller(controller);
	enums_dialog->set_transient_for(*this);

	factories_dialog->set_controller(controller);
	factories_dialog->set_transient_for(*this);

	klasses_dialog->set_controller(controller);
	klasses_dialog->set_transient_for(*this);

	properties_module->set_controller(controller);

	connection_properties_dialog->set_transient_for(*this);
}

void MainWindow::connectionPropertiesMenuItem_activate_cb()
{
	connection_properties_dialog->show();
}

void MainWindow::connectMenuItem_activate_cb()
{
	if (controller->is_connected())
		controller->disconnect();
	else
		controller->connect(
			connection_properties_dialog->get_ip_address(),
			connection_properties_dialog->get_port());
}

void MainWindow::connection_status_changed(bool connected)
{
	auto id = main_statusbar->get_context_id("connection-status");
	if (connected)
	{
		main_statusbar->push(_("Connected"), id);
		((Gtk::Label*)connect_menu_item->get_child())->set_text(_("Disconnect"));
	}
	else
	{
		main_statusbar->push(_("Disconnected"), id);
		((Gtk::Label*)connect_menu_item->get_child())->set_text(_("Connect"));
	}
}

void MainWindow::addins_reloaded()
{
	static int plugin_val = 31337;
	Gtk::Menu *sub_menu = addins_menu_item->get_submenu();

	for (auto m : sub_menu->get_children())
		if (GPOINTER_TO_INT(m->get_data("plugin-menu-item")) == plugin_val)
			sub_menu->remove(*m);

	addins_menu_item->set_submenu(*sub_menu);

	for (auto addin : this->controller->get_addins())
	{
		auto v = Gtk::manage(new Gtk::MenuItem(addin->get_name(), true));
		v->set_data("plugin-menu-item", GINT_TO_POINTER(plugin_val));
		v->signal_activate().connect([addin] {
			if (addin->get_widget()->is_visible()) return;
			Gtk::Window *wnd = new Gtk::Window();
			wnd->add(*addin->get_widget());
			wnd->set_title(addin->get_name());
			wnd->signal_delete_event().connect([wnd](GdkEventAny *){ delete wnd; return true; });
			wnd->show_all();
		});
		sub_menu->prepend(*v);
	}
	sub_menu->show_all();
}
