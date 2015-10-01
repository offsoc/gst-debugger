/*
 * klasses_dialog.h
 *
 *  Created on: Oct 1, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_DIALOGS_KLASSES_DIALOG_H_
#define SRC_GST_DEBUGGER_DIALOGS_KLASSES_DIALOG_H_

#include "remote_data_dialog.h"

#include "controller/iview.h"

#include <gtkmm.h>

class KlassModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

	KlassModelColumns() { add(m_col_name); add(m_col_value); }

	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<Glib::ustring> m_col_value;
};

class KlassesDialog : public RemoteDataDialog
{
	KlassModelColumns klasses_columns;
	Glib::RefPtr<Gtk::TreeStore> klasses_tree_model;

	void reload_list(const Glib::ustring &klass_name, bool add);

	std::string g_param_flags_to_string(GParamFlags v, std::string flags_name);

public:
	KlassesDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_DIALOGS_KLASSES_DIALOG_H_ */
