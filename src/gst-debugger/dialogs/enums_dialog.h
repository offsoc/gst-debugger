/*
 * enums_dialog.h
 *
 *  Created on: Jun 26, 2015
 *      Author: loganek
 */

#ifndef SRC_ENUMS_DIALOG_H_
#define SRC_ENUMS_DIALOG_H_

#include "controller/iview.h"

#include <gtkmm.h>

class ModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

	ModelColumns() { add(m_col_name); add(m_col_value); }

	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<int> m_col_value;
};

class EnumsDialog : public Gtk::Dialog, public IBaseView
{
	Gtk::TreeView *enum_types_tree_view;

	ModelColumns m_Columns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

	void reload_list();

public:
	EnumsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_ENUMS_DIALOG_H_ */
