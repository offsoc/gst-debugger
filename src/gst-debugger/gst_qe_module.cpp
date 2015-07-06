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

GstQEModule::GstQEModule(bool type_module, bool pad_path_module,
		GstreamerInfo_InfoType info_type,
		const std::string& qe_name, GType qe_gtype, const Glib::RefPtr<Gtk::Builder>& builder,
		const std::shared_ptr<GstDebuggerTcpClient>& client)
: client(client),
  info_type(info_type),
  type_module(type_module)
{
	builder->get_widget("existing" + qe_name + "HooksTreeView", existing_hooks_tree_view);
	qe_hooks_model = Gtk::ListStore::create(qe_hooks_model_columns);
	existing_hooks_tree_view->set_model(qe_hooks_model);

	if (pad_path_module)
	{
		builder->get_widget("any" + qe_name + "PathCheckButton", any_path_check_button);
		any_path_check_button->signal_toggled().connect([this] { qe_pad_path_entry->set_sensitive(!any_path_check_button->get_active()); });
		builder->get_widget("pad" + qe_name + "PathEntry", qe_pad_path_entry);
		existing_hooks_tree_view->append_column("Pad path", qe_hooks_model_columns.pad_path);
	}

	if (type_module)
	{
		builder->get_widget("any" + qe_name + "CheckButton", any_qe_check_button);
		any_qe_check_button->signal_toggled().connect([this] { qe_types_combobox->set_sensitive(!any_qe_check_button->get_active()); });

		builder->get_widget("types" + qe_name + "ComboBox", qe_types_combobox);
		qe_types_model = Gtk::ListStore::create(qe_types_model_columns);
		qe_types_combobox->set_model(qe_types_model);
		qe_types_combobox->pack_start(qe_types_model_columns.type_name);

		for (auto val : GValueEnum::get_values(qe_gtype))
		{
			Gtk::TreeModel::Row row = *(qe_types_model->append());
			row[qe_types_model_columns.type_id] = val.first;
			row[qe_types_model_columns.type_name] = val.second;
		}
		if (qe_types_model->children().size() > 0)
		{
			qe_types_combobox->set_active(0);
		}

		existing_hooks_tree_view->append_column(qe_name + " type", qe_hooks_model_columns.qe_type);
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

void GstQEModule::update_hook_list()
{
	auto conf = info.confirmation();

	if (conf.watch_type() != get_watch_type())
		return;

	if (conf.toggle() == ENABLE)
	{
		Gtk::TreeModel::Row row = *(qe_hooks_model->append());
		row[qe_hooks_model_columns.pad_path] = conf.pad_path();
		row[qe_hooks_model_columns.qe_type] = conf.qe_type();
	}
	else
	{
		for (auto iter = qe_hooks_model->children().begin();
				iter != qe_hooks_model->children().end(); ++iter)
		{
			if ((*iter)[qe_hooks_model_columns.pad_path] == conf.pad_path() &&
					(*iter)[qe_hooks_model_columns.qe_type] == conf.qe_type())
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

	std::string pad_path = any_path_check_button->get_active() ? Glib::ustring() : qe_pad_path_entry->get_text();

	Command cmd;
	PadWatch *pad_watch = new PadWatch();
	pad_watch->set_toggle(enable ? ENABLE : DISABLE);
	pad_watch->set_watch_type(get_watch_type());
	pad_watch->set_pad_path(pad_path);
	pad_watch->set_qe_type(qe_type);
	cmd.set_command_type(Command_CommandType_PAD_WATCH);
	cmd.set_allocated_pad_watch(pad_watch);
	client->send_command(cmd);
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
	display_qe_details(qe);
}

void GstQEModule::display_qe_details(const Glib::RefPtr<Gst::MiniObject>& qe)
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
		auto gvalue = GValueBase::build_gvalue(const_cast<GValue*>(value.gobj()));
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

void GstQEModule::process_frame()
{
	if (info.info_type() == info_type)
		append_qe_entry();
	else if (info.info_type() == GstreamerInfo_InfoType_PAD_WATCH_CONFIRMATION ||
			info.info_type() == GstreamerInfo_InfoType_MESSAGE_CONFIRMATION)
		update_hook_list();
}

void GstQEModule::startWatchingQEButton_click_cb()
{
	send_start_stop_command(true);
}

void GstQEModule::stopWatchingQEButton_click_cb()
{
	send_start_stop_command(false);
}

