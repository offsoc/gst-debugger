/*
 * gst_query_module.cpp
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#include "gst_query_module.h"

extern "C" {
#include "protocol/deserializer.h"
}

GstQueryModule::GstQueryModule(const Glib::RefPtr<Gtk::Builder>& builder, const std::shared_ptr<GstDebuggerTcpClient>& client)
: GstQEModule(GstreamerInfo_InfoType_QUERY, PadWatch_WatchType_QUERY,
		"Query", gst_query_type_get_type(), builder, client)
{
}

void GstQueryModule::append_qe_entry()
{
	auto gstquery = info.qebm();

	GstQuery *query = gst_query_deserialize(gstquery.payload().c_str(), gstquery.payload().length());

	if (query == NULL)
	{
		// todo log about it
		return;
	}

	Gtk::TreeModel::Row row = *(qe_list_model->append());
	row[qe_list_model_columns.type] = query->type;
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

