/*
 * factories_dialog.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: loganek
 */

#include "factories_dialog.h"
#include "ui_utils.h"

#include "controller/controller.h"

FactoriesDialog::FactoriesDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: RemoteDataDialog(cobject, builder)
{
	tree_model = Gtk::TreeStore::create(factories_columns);
	data_tree_view->set_model(tree_model);
	data_tree_view->append_column("Property", factories_columns.m_col_name);
	data_tree_view->append_column("Value", factories_columns.m_col_value);

	set_title("Remote Factories");
}

void FactoriesDialog::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_factory_list_changed.connect(sigc::mem_fun(*this, &FactoriesDialog::reload_list));
	reload_list("");
}

// todo we don't need reload whole list
void FactoriesDialog::reload_list(const Glib::ustring &factory_name)
{
	tree_model->clear();

#define APPEND_SUB_ROW(name, value, parent) \
	[=] { \
		auto childrow = *(tree_model->append(parent.children())); \
		childrow[factories_columns.m_col_name] = name; \
		childrow[factories_columns.m_col_value] = value; \
		return childrow; \
	} ()

	for (auto factory : controller->get_factory_container())
	{
		auto row = *(tree_model->append());
		row[factories_columns.m_col_name] = factory.get_name();
		row[factories_columns.m_col_value] = "";

		for (auto meta : factory.get_metadata())
		{
			APPEND_SUB_ROW(meta.first, meta.second, row);
		}

		auto tpl_row = APPEND_SUB_ROW("Pad templates", "", row);

		for (auto tpl : factory.get_pad_templates())
		{
			display_template_info(tpl, tree_model, factories_columns.m_col_name, factories_columns.m_col_value, tpl_row);
		}
	}

#undef APPEND_SUB_ROW
}


