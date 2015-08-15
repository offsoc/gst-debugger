/*
 * gst_qe_module.cpp
 *
 *  Created on: Jul 2, 2015
 *      Author: mkolny
 */

#include "gst_qe_module.h"
#include "sigc++lambdahack.h"
#include "gvalue-converter/gvalue_base.h"
#include "gvalue-converter/gvalue_enum.h"
#include "protocol/deserializer.h"
#include "controller/controller.h"
#include "controller/element_path_processor.h"

template<typename T>
static void free_data(T *data) { delete data; }

GstQEModule::GstQEModule(bool type_module, bool pad_path_module,
		GstreamerInfo_InfoType info_type,
		const std::string& qe_name, GType qe_gtype, const Glib::RefPtr<Gtk::Builder>& builder)
:  qe_gtype(qe_gtype),
   info_type(info_type),
  type_module(type_module),
  pad_path_module(pad_path_module)
{
	builder->get_widget("existing" + qe_name + "HooksTreeView", existing_hooks_tree_view);
	qe_hooks_model = Gtk::ListStore::create(qe_hooks_model_columns);
	existing_hooks_tree_view->set_model(qe_hooks_model);

	if (pad_path_module)
	{
		builder->get_widget("any" + qe_name + "PathCheckButton", any_path_check_button);
		any_path_check_button->signal_clicked().connect([this]{ set_start_watch_button_sensitive(); });
		existing_hooks_tree_view->append_column("Pad path", qe_hooks_model_columns.pad_path);
		builder->get_widget("padPath" + qe_name + "Label", pad_path_label);
	}

	if (type_module)
	{
		builder->get_widget("any" + qe_name + "CheckButton", any_qe_check_button);
		any_qe_check_button->signal_toggled().connect([this] { qe_types_combobox->set_sensitive(!any_qe_check_button->get_active()); });

		builder->get_widget("types" + qe_name + "ComboBox", qe_types_combobox);
		qe_types_model = Gtk::ListStore::create(qe_types_model_columns);
		qe_types_combobox->set_model(qe_types_model);
		qe_types_combobox->pack_start(qe_types_model_columns.type_name);

		existing_hooks_tree_view->append_column(qe_name + " type", qe_hooks_model_columns.qe_type_name);
	}

	builder->get_widget(qe_name + "ListTreeView", qe_list_tree_view);
	qe_list_tree_view->signal_row_activated().connect(sigc::mem_fun(*this, &GstQEModule::qeListTreeView_row_activated_cb));
	qe_list_model = Gtk::ListStore::create(qe_list_model_columns);
	qe_list_tree_view->set_model(qe_list_model);
	qe_list_tree_view->append_column("", qe_list_model_columns.type);

	builder->get_widget("details" + qe_name + "TreeView", qe_details_tree_view);
	qe_details_model = Gtk::TreeStore::create(qe_details_model_columns);
	qe_details_tree_view->set_model(qe_details_model);
	qe_details_tree_view->append_column("Name", qe_details_model_columns.name);
	qe_details_tree_view->append_column("Value", qe_details_model_columns.value);

	builder->get_widget("startWatching" + qe_name + "Button", start_watching_qe_button);
	start_watching_qe_button->signal_clicked().connect(sigc::mem_fun(*this, &GstQEModule::startWatchingQEButton_click_cb));

	builder->get_widget("removeSelected" + qe_name + "Hook", remove_selected_hook_button);
	remove_selected_hook_button->signal_clicked().connect(sigc::mem_fun(*this, &GstQEModule::removeSelectedHook_click_cb));

	create_dispatcher("qebm", sigc::mem_fun(*this, &GstQEModule::qebm_received_), (GDestroyNotify)free_data<GstreamerQEBM>);
	create_dispatcher("confirmation", sigc::mem_fun(*this, &GstQEModule::confirmation_received_),
			info_type == GstreamerInfo_InfoType_MESSAGE ? (GDestroyNotify)free_data<MessageWatch> : (GDestroyNotify)free_data<PadWatch>);
	create_dispatcher("enum", sigc::mem_fun(*this, &GstQEModule::enum_list_changed_), (GDestroyNotify)free_data<std::string>);

	create_dispatcher("selected-object", sigc::mem_fun(*this, &GstQEModule::selected_object_changed), nullptr);

}

void GstQEModule::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_qebm_received.connect(sigc::mem_fun(*this, &GstQEModule::qebm_received));

	controller->on_enum_list_changed.connect(sigc::mem_fun(*this, &GstQEModule::enum_list_changed));

	if (info_type != GstreamerInfo_InfoType_MESSAGE)
	{
		controller->on_pad_watch_confirmation_received.connect(sigc::mem_fun(*this, &GstQEModule::pad_confirmation_received));
		controller->on_selected_object_changed.connect([this]{gui_emit("selected-object");});
	}
}

void GstQEModule::enum_list_changed(const Glib::ustring &enum_name)
{
	if ((qe_gtype == gst_query_type_get_type() && enum_name == "GstQueryType") ||
			(qe_gtype == gst_event_type_get_type() && enum_name == "GstEventType") ||
			(qe_gtype == gst_message_type_get_type() && enum_name == "GstMessageType"))
	{
		gui_push("enum", new std::string(enum_name));
		gui_emit("enum");
	}
}

void GstQEModule::enum_list_changed_()
{
	qe_types_model->clear();
	std::string* type_name = gui_pop<std::string*>("enum");
	GstEnumType type = const_cast<RemoteDataContainer<GstEnumType>&>(controller->get_enum_container()).get_item(*type_name);
	for (auto val : type.get_values())
	{
		Gtk::TreeModel::Row row = *(qe_types_model->append());
		row[qe_types_model_columns.type_id] = val.first;
		row[qe_types_model_columns.type_name] = val.second.nick;
	}
	if (qe_types_model->children().size() > 0)
	{
		qe_types_combobox->set_active(0);
	}
	delete type_name;
}

PadWatch_WatchType GstQEModule::get_watch_type() const
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

void GstQEModule::update_hook_list(PadWatch *conf)
{
	if (conf->watch_type() != get_watch_type())
		return;

	if (conf->toggle() == ENABLE)
	{
		Gtk::TreeModel::Row row = *(qe_hooks_model->append());
		row[qe_hooks_model_columns.pad_path] = conf->pad_path();

		if (get_watch_type() != PadWatch_WatchType_BUFFER)
		{
			row[qe_hooks_model_columns.qe_type_name] = get_watch_type() == PadWatch_WatchType_EVENT ?
					Gst::Enums::get_name(static_cast<Gst::EventType>(conf->qe_type())) :
					Gst::Enums::get_name(static_cast<Gst::QueryType>(conf->qe_type()));
		}
		row[qe_hooks_model_columns.qe_type] = conf->qe_type();
	}
	else
	{
		for (auto iter = qe_hooks_model->children().begin();
				iter != qe_hooks_model->children().end(); ++iter)
		{
			if ((*iter)[qe_hooks_model_columns.pad_path] == conf->pad_path() &&
					(*iter)[qe_hooks_model_columns.qe_type] == conf->qe_type())
			{
				qe_hooks_model->erase(iter);
				break;
			}
		}
	}
}

void GstQEModule::send_start_stop_command(bool enable)
{
	int qe_type = -1;

	if (type_module && !any_qe_check_button->get_active())
	{
		Gtk::TreeModel::iterator iter = qe_types_combobox->get_active();
		if (!iter)
			return;

		Gtk::TreeModel::Row row = *iter;
		if (!row)
			return;
		qe_type = row[qe_types_model_columns.type_id];
	}

	auto selected_pad = std::dynamic_pointer_cast<PadModel>(controller->get_selected_object());

	if (!selected_pad && !any_path_check_button->get_active())
	{
		return;
	}

	std::string pad_path = any_path_check_button->get_active() ? std::string() : ElementPathProcessor::get_object_path(selected_pad);

	controller->send_pad_watch_command(enable, get_watch_type(), pad_path, qe_type);
}

void GstQEModule::qeListTreeView_row_activated_cb(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
	Gtk::TreeModel::iterator iter = qe_list_model->get_iter(path);
	if (!iter)
	{
		return;
	}

	Gtk::TreeModel::Row row = *iter;
	Glib::RefPtr<Gst::MiniObject> qe = Glib::wrap(row[qe_list_model_columns.qe], true);
	display_qe_details(qe, (Glib::ustring)row[qe_list_model_columns.pad_path]);
}

void GstQEModule::display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe, const std::string &pad_path)
{
	qe_details_model->clear();
}

void GstQEModule::append_details_row(const std::string &name, const std::string &value)
{
	Gtk::TreeModel::Row row = *(qe_details_model->append());
	row[qe_details_model_columns.name] = name;
	row[qe_details_model_columns.value] = value;
}

void GstQEModule::append_details_from_structure(Gst::Structure& structure)
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

void GstQEModule::qebm_received(const GstreamerQEBM &qebm, GstreamerInfo_InfoType type)
{
	if (type == info_type)
	{
		gui_push("qebm", new GstreamerQEBM(qebm));
		gui_emit("qebm");
	}
}

void GstQEModule::qebm_received_()
{
	auto qebm = gui_pop<GstreamerQEBM*>("qebm");
	append_qe_entry(qebm);
	delete qebm;
}

void GstQEModule::pad_confirmation_received(const PadWatch& watch, PadWatch_WatchType type)
{
	if (type == get_watch_type())
	{
		gui_push("confirmation", new PadWatch(watch));
		gui_emit("confirmation");
	}
}

void GstQEModule::confirmation_received_()
{
	auto confirmation = gui_pop<PadWatch*>("confirmation");
	update_hook_list(confirmation);
	delete confirmation;
}

void GstQEModule::startWatchingQEButton_click_cb()
{
	send_start_stop_command(true);
}

void GstQEModule::removeSelectedHook_click_cb()
{
	send_start_stop_command(false);
}

void GstQEModule::selected_object_changed()
{
	std::shared_ptr<PadModel> selected_pad = std::dynamic_pointer_cast<PadModel>(controller->get_selected_object());

	if (selected_pad)
	{
		pad_path_label->set_text(ElementPathProcessor::get_object_path(selected_pad));
	}
	else
	{
		pad_path_label->set_text("(none)");
	}
	set_start_watch_button_sensitive();
}

void GstQEModule::set_start_watch_button_sensitive()
{
	bool enabled = pad_path_module ?
			std::dynamic_pointer_cast<PadModel>(controller->get_selected_object()) || any_path_check_button->get_active() :
			true;

	start_watching_qe_button->set_sensitive(enabled);
}
