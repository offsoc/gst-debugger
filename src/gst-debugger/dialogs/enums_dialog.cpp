/*
 * enums_dialog.cpp
 *
 *  Created on: Jun 26, 2015
 *      Author: loganek
 */

#include "enums_dialog.h"

#include "controller/controller.h"

EnumsDialog::EnumsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Dialog(cobject)
{
	builder->get_widget("enumTypesTreeView", enum_types_tree_view);
	enums_tree_model = Gtk::TreeStore::create(enums_columns);
	enum_types_tree_view->set_model(enums_tree_model);
	enum_types_tree_view->append_column("Name", enums_columns.m_col_name);
	enum_types_tree_view->append_column("Value", enums_columns.m_col_value);

	builder->get_widget("remoteEnumsCloseButton", close_button);
	close_button->signal_clicked().connect([this]{hide();});
}

void EnumsDialog::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_enum_list_changed.connect(sigc::mem_fun(*this, &EnumsDialog::reload_list));
	reload_list();
}

void EnumsDialog::reload_list()
{
	enums_tree_model->clear();

	for (auto enum_type : controller->get_enum_container())
	{
		auto row = *(enums_tree_model->append());
		row[enums_columns.m_col_name] = enum_type.get_type_name();
		row[enums_columns.m_col_value] = -1;

		for (auto enum_entry : enum_type.get_values())
		{
			auto childrow = *(enums_tree_model->append(row.children()));
			childrow[enums_columns.m_col_name] = enum_entry.second;
			childrow[enums_columns.m_col_value] = enum_entry.first;
		}
	}
}
