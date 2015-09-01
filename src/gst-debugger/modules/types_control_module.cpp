/*
 * types_control_module.cpp
 *
 *  Created on: Sep 1, 2015
 *      Author: loganek
 */

#include "types_control_module.h"

#include "controller/controller.h"

void TypesControlModule::append_types_widgets()
{
	Gtk::Box *type_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
	any_type_check_button = Gtk::manage(new Gtk::CheckButton("any event"));
	any_type_check_button->signal_clicked().connect([this] { update_add_hook(); });

	type_box->pack_start(*any_type_check_button);
	main_box->pack_start(*type_box);

	types_combobox = Gtk::manage(new Gtk::ComboBox());
	main_box->pack_start(*Gtk::manage(types_combobox), false, true);
	types_model = Gtk::ListStore::create(types_model_columns);
	types_combobox->set_model(types_model);
	types_combobox->pack_start(types_model_columns.type_name);

}

bool TypesControlModule::add_hook_unlocked()
{
	return any_type_check_button->get_active() ||
			!!types_combobox->get_active();
}

TypesControlModule::TypesControlModule(const std::string &enum_type_name)
: type_name(enum_type_name)
{
	create_dispatcher("enum", sigc::mem_fun(*this, &TypesControlModule::enum_list_changed_), nullptr); // todo memleak

	main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)); // todo possibly memleak

	append_types_widgets();
	append_hook_widgets();
}

Gtk::Widget* TypesControlModule::get_widget()
{
	return main_box;
}

void TypesControlModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_enum_list_changed.connect([this](const Glib::ustring &enum_name, bool add){
		if (enum_name != type_name)
			return;

		gui_push("enum", new std::string(enum_name));
		gui_push("enum", new bool(add));
		gui_emit("enum");
	});
}

void TypesControlModule::enum_list_changed_()
{
	types_model->clear();
	std::string* type_name = gui_pop<std::string*>("enum");
	bool *add = gui_pop<bool*>("enum");

	if (*add)
	{
		GstEnumType type = const_cast<RemoteDataContainer<GstEnumType>&>(controller->get_enum_container()).get_item(*type_name);
		for (auto val : type.get_values())
		{
			Gtk::TreeModel::Row row = *(types_model->append());
			row[types_model_columns.type_id] = val.first;
			row[types_model_columns.type_name] = val.second.nick;
		}
		if (types_model->children().size() > 0)
		{
			types_combobox->set_active(0);
		}
	}
	delete add;
	delete type_name;
}
