/*
 * factories_dialog.h
 *
 *  Created on: Aug 13, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_DIALOGS_FACTORIES_DIALOG_H_
#define SRC_GST_DEBUGGER_DIALOGS_FACTORIES_DIALOG_H_

#include "remote_data_dialog.h"

#include "controller/iview.h"

#include <gtkmm.h>

class FactoryModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

	FactoryModelColumns() { add(m_col_name); add(m_col_value); }

	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<Glib::ustring> m_col_value;
};

class FactoriesDialog : public RemoteDataDialog
{
	FactoryModelColumns factories_columns;
	Glib::RefPtr<Gtk::TreeStore> factories_tree_model;

	void reload_list(const Glib::ustring &factory_name);

public:
	FactoriesDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_DIALOGS_FACTORIES_DIALOG_H_ */
