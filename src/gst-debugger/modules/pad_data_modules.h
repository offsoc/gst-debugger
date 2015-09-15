/*
 * pad_data_modules.h
 *
 *  Created on: Aug 31, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_PAD_DATA_MODULES_H_
#define SRC_GST_DEBUGGER_MODULES_PAD_DATA_MODULES_H_

#include "base_main_module.h"

#include "common/gstdebugger.pb.h"

template<typename T>
class PadDataListModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	PadDataListModelColumns() {
		add(header); add(object); add(pad_path);
	}

	Gtk::TreeModelColumn<Glib::ustring> header;
	Gtk::TreeModelColumn<T*> object;
	Gtk::TreeModelColumn<Glib::ustring> pad_path;
};

template<typename T>
class PadDataModule : public BaseMainModule
{
	GstreamerInfo_InfoType info_type;

	virtual T* deserialize(const std::string &payload) = 0;

	void confirmation_received_();
	void qebm_received_();

protected:
	PadDataListModelColumns<T> columns;

	PadWatch_WatchType get_watch_type() const;

	virtual void display_details(const Glib::RefPtr<Gst::MiniObject> &obj, const Glib::ustring &pad_path) = 0;

	bool filter_function(const Gtk::TreeModel::const_iterator& it) override;

	virtual const GstStructure* get_gst_structure(const Gtk::TreeModel::const_iterator& it) const { return nullptr; }

public:
	PadDataModule(GstreamerInfo_InfoType info_type);

	virtual ~PadDataModule() {}

	void set_controller(const std::shared_ptr<Controller> &controller) override;
	void configure_main_list_view(Gtk::TreeView *view) override;
	void load_details(Gtk::TreeView *view, const Gtk::TreeModel::Path &path) override;
};

class EventModule : public PadDataModule<GstEvent>
{
	GstEvent *deserialize(const std::string &payload) override;

	void display_details(const Glib::RefPtr<Gst::MiniObject>& obj, const Glib::ustring &pad_path) override;

	const GstStructure* get_gst_structure(const Gtk::TreeModel::const_iterator &it) const override;

public:
	EventModule() : PadDataModule<GstEvent>(GstreamerInfo_InfoType_EVENT) {}

};

class QueryModule : public PadDataModule<GstQuery>
{
	GstQuery *deserialize(const std::string &payload) override;

	void display_details(const Glib::RefPtr<Gst::MiniObject>& obj, const Glib::ustring &pad_path) override;

	const GstStructure* get_gst_structure(const Gtk::TreeModel::const_iterator &it) const override;

public:
	QueryModule() : PadDataModule<GstQuery>(GstreamerInfo_InfoType_QUERY) {}
};

class BufferDataDialog;

class BufferModule : public PadDataModule<GstBuffer>
{
	BufferDataDialog *data_dialog;
	Glib::RefPtr<Gst::Buffer> buffer;

	GstBuffer *deserialize(const std::string &payload) override;

	void display_details(const Glib::RefPtr<Gst::MiniObject>& obj, const Glib::ustring &pad_path) override;
	void details_activated(const Gtk::TreeModel::Path &path) override;

public:
	BufferModule();
};

#endif /* SRC_GST_DEBUGGER_MODULES_PAD_DATA_MODULES_H_ */
