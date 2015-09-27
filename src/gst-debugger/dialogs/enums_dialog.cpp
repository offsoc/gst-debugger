/*
 * enums_dialog.cpp
 *
 *  Created on: Jun 26, 2015
 *      Author: loganek
 */

#include "enums_dialog.h"

#include "ui_utils.h"

#include "controller/controller.h"

EnumsDialog::EnumsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: RemoteDataDialog(cobject, builder)
{
	tree_model = Gtk::TreeStore::create(enums_columns);
	data_tree_view->set_model(tree_model);
	data_tree_view->append_column("Name", enums_columns.m_col_name);
	data_tree_view->append_column("Value", enums_columns.m_col_value);
	data_tree_view->append_column("Description", enums_columns.m_col_description);

	set_title("Remote Enum Types");
}

void EnumsDialog::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_enum_list_changed.connect(sigc::mem_fun(*this, &EnumsDialog::reload_list));
	reload_list("", true);
}

void EnumsDialog::reload_list(const Glib::ustring& enum_name, bool add)
{
	tree_model->clear();

	for (auto enum_type : controller->get_enums())
	{
		auto row = *(tree_model->append());
		row[enums_columns.m_col_name] = enum_type.get_name();
		row[enums_columns.m_col_value] = "";
		row[enums_columns.m_col_description] = "";

		for (auto enum_entry : enum_type.get_values())
		{
			auto childrow = *(tree_model->append(row.children()));
			childrow[enums_columns.m_col_name] = enum_entry.second.nick;
			childrow[enums_columns.m_col_value] = enum_type.get_gtype() == G_TYPE_ENUM ? std::to_string(enum_entry.first) : flags_value_to_string(enum_entry.first);
			childrow[enums_columns.m_col_description] = enum_entry.second.name;
		}
	}
}
