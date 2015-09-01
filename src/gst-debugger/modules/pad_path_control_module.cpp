/*
 * pad_path_control_module.cpp
 *
 *  Created on: Sep 1, 2015
 *      Author: loganek
 */

#include "pad_path_control_module.h"

#include "controller/controller.h"
#include "controller/element_path_processor.h"

template<typename T>
static void free_data(T *data) { delete data; }

void PadPathControlModule::append_pad_path_widgets()
{
	main_box->pack_start(*path_box, false, true);
	main_box->pack_start(*Gtk::manage(new Gtk::Label("none")), false, true);
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
	path_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
	path_box->pack_start(*Gtk::manage(new Gtk::Label("Pad path")));

	any_path_check_button = Gtk::manage(new Gtk::CheckButton("any path"));
	any_path_check_button->signal_clicked().connect([this] { update_add_hook(); });

	path_box->pack_start(*any_path_check_button);
	hooks_tree_view->append_column("Pad path", hooks_model_columns.pad_path);

	create_dispatcher("confirmation", sigc::mem_fun(*this, &PadPathControlModule::confirmation_received_), (GDestroyNotify)free_data<PadWatch>);
	create_dispatcher("selected-object", sigc::mem_fun(*this, &PadPathControlModule::selected_object_changed_), nullptr);
}

Gtk::Widget* PadPathControlModule::get_widget()
{
	if (main_box == nullptr)
	{
		main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)); // todo possibly memleak

		append_pad_path_widgets();
		append_hook_widgets();
	}
	return main_box;
}

void PadPathControlModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_selected_object_changed.connect([this](){
		gui_emit("selected-object");
	});

	controller->on_pad_watch_confirmation_received.connect([this](const PadWatch& watch, PadWatch_WatchType type) {
		if (type == watch_type)
		{
			gui_push("confirmation", new PadWatch(watch));
			gui_emit("confirmation");
		}
	});
}

std::string PadPathControlModule::get_pad_path() const
{
	auto selected_pad = std::dynamic_pointer_cast<PadModel>(controller->get_selected_object());

	return any_path_check_button->get_active() || !selected_pad ? std::string() : ElementPathProcessor::get_object_path(selected_pad);
}

void PadPathControlModule::confirmation_received_()
{
	auto conf = gui_pop<PadWatch*>("confirmation");

	if (conf->watch_type() != watch_type)
		return;

	if (conf->toggle() == ENABLE)
	{
		Gtk::TreeModel::Row row = *(hooks_model->append());
		row[hooks_model_columns.pad_path] = conf->pad_path();

		if (watch_type != PadWatch_WatchType_BUFFER)
		{
			row[hooks_model_columns.qe_type_name] = watch_type == PadWatch_WatchType_EVENT ?
					Gst::Enums::get_name(static_cast<Gst::EventType>(conf->qe_type())) :
					Gst::Enums::get_name(static_cast<Gst::QueryType>(conf->qe_type()));
		}
		row[hooks_model_columns.qe_type] = conf->qe_type();
	}
	else
	{
		for (auto iter = hooks_model->children().begin();
				iter != hooks_model->children().end(); ++iter)
		{
			if ((*iter)[hooks_model_columns.pad_path] == conf->pad_path() &&
					(*iter)[hooks_model_columns.qe_type] == conf->qe_type())
			{
				hooks_model->erase(iter);
				break;
			}
		}
	}

	delete conf;
}
