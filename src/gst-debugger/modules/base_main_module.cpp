/*
 * base_main_module.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#include "base_main_module.h"

DetailsModelColumns BaseMainModule::detail_columns;

BaseMainModule::BaseMainModule()
{
	details_model = Gtk::ListStore::create(detail_columns);
}

void BaseMainModule::configure_main_list_view(Gtk::TreeView *view)
{
	view->remove_all_columns();
	view->set_model(model);
}

void BaseMainModule::load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path)
{
	details_model->clear();
	view->set_model(details_model);
}

void BaseMainModule::configure_details_view(Gtk::TreeView *view)
{
	view->remove_all_columns();
	view->append_column("Name", detail_columns.name);
	view->append_column("Value", detail_columns.value);
}

