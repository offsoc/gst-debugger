/*
 * pad_path_control_module.cpp
 *
 *  Created on: Sep 1, 2015
 *      Author: loganek
 */

#include "pad_path_control_module.h"

#include "controller/controller.h"
#include "controller/element_path_processor.h"

void PadPathControlModule::append_pad_path_widgets()
{
	Gtk::Box *path_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
	path_box->pack_start(*Gtk::manage(new Gtk::Label("Pad path")));

	any_path_check_button = Gtk::manage(new Gtk::CheckButton("any path"));
	any_path_check_button->signal_clicked().connect([this] { update_add_hook(); });

	path_box->pack_start(*any_path_check_button);
	main_box->pack_start(*path_box);
	main_box->pack_start(*Gtk::manage(new Gtk::Label("none")));
}

bool PadPathControlModule::add_hook_unlocked()
{
	return any_path_check_button->get_active() ||
			(controller && std::dynamic_pointer_cast<PadModel>(controller->get_selected_object()) != nullptr);
}

void PadPathControlModule::selected_object_changed_()
{
	update_add_hook();
}

PadPathControlModule::PadPathControlModule(PadWatch_WatchType watch_type)
: HooksControlModule(watch_type)
{
	main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)); // todo possibly memleak

	append_pad_path_widgets();
	append_hook_widgets();

	create_dispatcher("selected-object", sigc::mem_fun(*this, &PadPathControlModule::selected_object_changed_), nullptr);
}

Gtk::Widget* PadPathControlModule::get_widget()
{
	return main_box;
}

void PadPathControlModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_selected_object_changed.connect([this](){
		gui_emit("selected-object");
	});
}

std::string PadPathControlModule::get_pad_path() const
{
	auto selected_pad = std::dynamic_pointer_cast<PadModel>(controller->get_selected_object());

	return any_path_check_button->get_active() || !selected_pad ? std::string() : ElementPathProcessor::get_object_path(selected_pad);
}
