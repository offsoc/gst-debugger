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
	m_refTreeModel = Gtk::TreeStore::create(m_Columns);
	enum_types_tree_view->set_model(m_refTreeModel);
	enum_types_tree_view->append_column("Name", m_Columns.m_col_name);
	enum_types_tree_view->append_column("Value", m_Columns.m_col_value);
}

void EnumsDialog::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_enum_list_changed.connect(sigc::mem_fun(*this, &EnumsDialog::reload_list));
	reload_list();
}

void EnumsDialog::reload_list()
{
	m_refTreeModel->clear();

	for (auto enum_type : controller->get_enum_container())
	{
		auto row = *(m_refTreeModel->append());
		row[m_Columns.m_col_name] = enum_type.get_type_name();
		row[m_Columns.m_col_value] = -1;

		for (auto enum_entry : enum_type.get_values())
		{
			auto childrow = *(m_refTreeModel->append(row.children()));
			childrow[m_Columns.m_col_name] = enum_entry.second;
			childrow[m_Columns.m_col_value] = enum_entry.first;
		}
	}
}
