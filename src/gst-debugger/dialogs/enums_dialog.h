/*
 * enums_dialog.h
 *
 *  Created on: Jun 26, 2015
 *      Author: loganek
 */

#ifndef SRC_ENUMS_DIALOG_H_
#define SRC_ENUMS_DIALOG_H_

#include "remote_data_dialog.h"

#include "controller/iview.h"

#include <gtkmm.h>

class EnumsModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

	EnumsModelColumns() { add(m_col_name); add(m_col_value); add(m_col_description); }

	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<Glib::ustring> m_col_value;
	Gtk::TreeModelColumn<Glib::ustring> m_col_description;
};

class EnumsDialog : public RemoteDataDialog
{
	EnumsModelColumns enums_columns;

	void reload_list(const Glib::ustring &enum_name, bool add);

public:
	EnumsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_ENUMS_DIALOG_H_ */
