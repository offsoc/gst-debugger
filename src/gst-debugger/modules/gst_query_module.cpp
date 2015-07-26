/*
 * gst_query_module.cpp
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#include "gst_query_module.h"
#include "protocol/deserializer.h"

GstQueryModule::GstQueryModule(const Glib::RefPtr<Gtk::Builder>& builder)
: GstQEModule(true, true, GstreamerInfo_InfoType_QUERY,
		"Query", gst_query_type_get_type(), builder)
{
}

void GstQueryModule::append_qe_entry(GstreamerQEBM *qebm)
{
	GstQuery *query = gst_query_deserialize(qebm->payload().c_str(), qebm->payload().length());

	if (query == NULL)
	{
		// todo log about it
		return;
	}

	Gtk::TreeModel::Row row = *(qe_list_model->append());
	row[qe_list_model_columns.type] = std::string("Query ") + Gst::Enums::get_name(static_cast<Gst::QueryType>(query->type));
	row[qe_list_model_columns.qe] = GST_MINI_OBJECT(query);
}

void GstQueryModule::display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe)
{
	GstQEModule::display_qe_details(qe);

	Glib::RefPtr<Gst::Query> query = query.cast_static(qe);

	append_details_row("query type", Gst::Enums::get_name(query->get_query_type()));

	auto structure = query->get_structure();
	append_details_from_structure(structure);
}

