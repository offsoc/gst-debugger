/*
 * remote_data_dialog.h
 *
 *  Created on: Aug 13, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_DIALOGS_REMOTE_DATA_DIALOG_H_
#define SRC_GST_DEBUGGER_DIALOGS_REMOTE_DATA_DIALOG_H_

#include "controller/iview.h"

#include <gtkmm.h>

class RemoteDataDialog : public Gtk::Dialog, public IBaseView
{
	Gtk::Button *close_button;

protected:
	Gtk::TreeView *data_tree_view;
	Glib::RefPtr<Gtk::TreeStore> tree_model;

public:
	RemoteDataDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
	: Gtk::Dialog(cobject)
	{
		builder->get_widget("dataTreeView", data_tree_view);

		builder->get_widget("remoteDataCloseButton", close_button);
		close_button->signal_clicked().connect([this]{hide();});
	}
};

#endif /* SRC_GST_DEBUGGER_DIALOGS_REMOTE_DATA_DIALOG_H_ */
