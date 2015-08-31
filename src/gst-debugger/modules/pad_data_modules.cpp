/*
 * pad_data_modules.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#include "pad_data_modules.h"

#include "dialogs/buffer_data_dialog.h"

#include "controller/controller.h"

#include "common/deserializer.h"

template<typename T>
static void free_data(T *data) { delete data; }

template<typename T>
PadDataModule<T>::PadDataModule(GstreamerInfo_InfoType info_type)
: info_type(info_type)
{
	model = Gtk::ListStore::create(columns);
	create_dispatcher("qebm", sigc::mem_fun(*this, &PadDataModule::qebm_received_), (GDestroyNotify)free_data<GstreamerQEBM>);
}

template<typename T>
void PadDataModule<T>::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);

	controller->on_qebm_received.connect([this](const GstreamerQEBM& qebm, GstreamerInfo_InfoType type){
		if (type == info_type)
		{
			gui_push("qebm", new GstreamerQEBM(qebm));
			gui_emit("qebm");
		}
	});
}

template<typename T>
void PadDataModule<T>::qebm_received_()
{
	auto qebm = gui_pop<GstreamerQEBM*>("qebm");
	Gtk::TreeModel::Row row = *(model->append());
	row[columns.pad_path] = qebm->pad_path();
	row[columns.object] = deserialize(qebm->payload());
	row[columns.header] = "Event"; // todo
	delete qebm;
}

template<typename T>
void PadDataModule<T>::configure_main_list_view(Gtk::TreeView *view)
{
	BaseMainModule::configure_main_list_view(view);
	view->append_column("PadData", columns.header); // todo
}

template<typename T>
void PadDataModule<T>::load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path)
{
	BaseMainModule::load_details(view, path);

	Gtk::TreeModel::iterator iter = model->get_iter(path);
	if (!iter)
	{
		return;
	}

	Gtk::TreeModel::Row row = *iter;
	display_details(Glib::wrap(row[columns.object], true), (Glib::ustring)row[columns.pad_path]);
}

template<typename T>
void PadDataModule<T>::append_details_row(const std::string &name, const std::string &value)
{
	Gtk::TreeModel::Row row = *(details_model->append());
	row[detail_columns.name] = name;
	row[detail_columns.value] = value;
}

template<typename T>
void PadDataModule<T>::append_details_from_structure(Gst::Structure& structure)
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

template<typename T>
PadWatch_WatchType PadDataModule<T>::get_watch_type() const
{
	switch (info_type)
	{
	case GstreamerInfo_InfoType_BUFFER:
		return PadWatch_WatchType_BUFFER;
	case GstreamerInfo_InfoType_EVENT:
		return PadWatch_WatchType_EVENT;
	case GstreamerInfo_InfoType_QUERY:
		return PadWatch_WatchType_QUERY;
	default:
		return (PadWatch_WatchType)-1;
	}
}

GstEvent* EventModule::deserialize(const std::string &payload)
{
	return gst_event_deserialize(payload.c_str(), payload.length());
}


void EventModule::display_details(const Glib::RefPtr<Gst::MiniObject>& obj, const Glib::ustring &pad_path)
{
	Glib::RefPtr<Gst::Event> event = event.cast_static(obj);

	append_details_row("pad path", pad_path);
	append_details_row("event type", Gst::Enums::get_name(event->get_event_type()));
	{
		gchar buffer[20];
		snprintf(buffer, 20, "%" GST_TIME_FORMAT, GST_TIME_ARGS(event->get_timestamp()));
		append_details_row("event timestamp", buffer);
	}
	append_details_row("event sequence number", std::to_string(event->get_seqnum()));

	auto structure = event->get_structure();
	append_details_from_structure(structure);
}

GstQuery* QueryModule::deserialize(const std::string &payload)
{
	return gst_query_deserialize(payload.c_str(), payload.length());
}

void QueryModule::display_details(const Glib::RefPtr<Gst::MiniObject>& obj, const Glib::ustring &pad_path)
{
	Glib::RefPtr<Gst::Query> query = query.cast_static(obj);

	append_details_row("pad path", pad_path);
	append_details_row("query type", Gst::Enums::get_name(query->get_query_type()));

	auto structure = query->get_structure();
	append_details_from_structure(structure);
}

GstBuffer* BufferModule::deserialize(const std::string &payload)
{
	return gst_buffer_deserialize(payload.c_str(), payload.length());
}

void BufferModule::display_details(const Glib::RefPtr<Gst::MiniObject>& obj, const Glib::ustring &pad_path)
{
	buffer = buffer.cast_static(obj);

	append_details_row("pad path", pad_path);
	append_details_row("pts", std::to_string(buffer->get_pts()));
	append_details_row("dts", std::to_string(buffer->get_dts()));
	append_details_row("duration", std::to_string(buffer->get_duration()));
	append_details_row("offset", std::to_string(buffer->get_offset()));
	append_details_row("offset_end", std::to_string(buffer->get_offset_end()));
	append_details_row("size", std::to_string(buffer->get_size()));
	append_details_row("flags", std::to_string(buffer->get_flags()));

	append_details_row("data", buffer_data_to_string(StringDataFormat::HEX, buffer, 1024, 16));
}

void BufferModule::details_activated(const Gtk::TreeModel::Path &path)
{
	Gtk::TreeModel::iterator iter = details_model->get_iter(path);
	if (!iter)
	{
		return;
	}

	Gtk::TreeModel::Row row = *iter;
	if (row[detail_columns.name] == "data")
	{
		data_dialog->set_buffer(buffer);
		data_dialog->show();
	}
}

BufferModule::BufferModule()
: PadDataModule<GstBuffer>(GstreamerInfo_InfoType_BUFFER)
{
	data_dialog = load_dialog<BufferDataDialog>("bufferDataDialog");
}

template class PadDataModule<GstEvent>;
template class PadDataModule<GstBuffer>;
template class PadDataModule<GstQuery>;
