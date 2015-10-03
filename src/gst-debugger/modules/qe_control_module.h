/*
 * qe_control_module.h
 *
 *  Created on: Sep 29, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_QE_CONTROL_MODULE_H_
#define SRC_GST_DEBUGGER_MODULES_QE_CONTROL_MODULE_H_

#include "control_module.h"

class QEControlModule : virtual public ControlModule
{
	const std::string enum_name;

protected:
	TypesModelColumns types_model_columns;
	Glib::RefPtr<Gtk::ListStore> types_model;

	Gtk::Label *pad_path_label;
	Gtk::ComboBox *types_combobox;

	bool hook_is_the_same(const Gtk::TreeModel::Row& row, gconstpointer confirmation) override
	{
		auto pad = reinterpret_cast<const GstDebugger::PadWatchRequest*>(confirmation);
		return row[hooks_model_columns.int1] == pad->event().type() && row[hooks_model_columns.str2] == pad->pad();
	}

public:
	QEControlModule(const std::string& enum_name)
	: enum_name(enum_name)
	{
		pad_path_label = Gtk::manage(new Gtk::Label());
		main_box->pack_start(*pad_path_label, false, true);
		main_box->reorder_child(*pad_path_label, 0);

		types_combobox = Gtk::manage(new Gtk::ComboBox());
		types_model = Gtk::ListStore::create(types_model_columns);
		types_combobox->set_model(types_model);
		types_combobox->pack_start(types_model_columns.type_name);
		main_box->pack_start(*types_combobox, false, true);
		main_box->reorder_child(*types_combobox, 0);

		hooks_tree_view->append_column("Type", hooks_model_columns.str1);
		hooks_tree_view->append_column("Pad", hooks_model_columns.str2);

		create_dispatcher("selected-object", [this] {
			auto pad_path = controller->get_selected_pad_path();
			if (pad_path.empty())
				pad_path = "none (any path)";
			pad_path_label->set_text(pad_path);
		}, nullptr);
	}

	void set_controller(const std::shared_ptr<Controller> &controller) override
	{
		ControlModule::set_controller(controller);

		controller->on_selected_object_changed.connect([this](){
			gui_emit("selected-object");
		});

		controller->on_enum_list_changed.connect([this](const Glib::ustring& name, bool add) {
			if (name != enum_name)
				return;
			types_model->clear();
			if (add)
			{
				boost::optional<GstEnumType> type = this->controller->get_enum_type(name);
				if (!type)
					return;

				for (auto t : type.get().get_values())
				{
					Gtk::TreeModel::Row row = *(types_model->append());
					row[types_model_columns.type_id] = t.first;
					row[types_model_columns.type_name] = t.second.name;
				}

				if (!type.get().get_values().empty())
					types_combobox->set_active(0);
			}
		});
	}
};

#endif /* SRC_GST_DEBUGGER_MODULES_QE_CONTROL_MODULE_H_ */
