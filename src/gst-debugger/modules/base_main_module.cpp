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

	filter = Gtk::TreeModelFilter::create(model);
	filter->set_visible_func([this](const Gtk::TreeModel::const_iterator& it){
		return filter_function(it);
	});

	view->set_model(filter);
}

void BaseMainModule::load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path)
{
	details_model->clear();
	view->set_model(details_model);
}

void BaseMainModule::update_filter_expression(const std::string &expr)
{
	Parser p;
	auto prev = filter_expression;
	try
	{
		filter_expression = std::make_shared<Expression>(p.parse(expr));
	}
	catch (...) { filter_expression = std::shared_ptr<Expression>(); }

	if (prev != filter_expression)
	{
		filter->refilter();
	}
}

void BaseMainModule::configure_details_view(Gtk::TreeView *view)
{
	view->remove_all_columns();
	view->append_column("Name", detail_columns.name);
	view->append_column("Value", detail_columns.value);
}

void BaseMainModule::append_details_row(const std::string &name, const std::string &value)
{
	Gtk::TreeModel::Row row = *(details_model->append());
	row[detail_columns.name] = name;
	row[detail_columns.value] = value;
}

void BaseMainModule::append_details_from_structure(Gst::Structure& structure)
{
	if (!structure.gobj())
		return;

	structure.foreach([structure, this](const Glib::ustring &name, const Glib::ValueBase &value) -> bool {
		GValue* tmp_val = new GValue;
		*tmp_val = G_VALUE_INIT;
		g_value_init(tmp_val, value.gobj()->g_type);
		g_value_copy(value.gobj(), tmp_val);
		auto gvalue = GValueBase::build_gvalue(tmp_val);
		if (gvalue == nullptr)
			append_details_row(name, std::string("<unsupported type ") + g_type_name(G_VALUE_TYPE(value.gobj())) + ">");
		else
		{
			append_details_row(name, gvalue->to_string());
			delete gvalue;
		}
		return true;
	});
}
