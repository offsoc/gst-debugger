/*
 * gst_properties_module.h
 *
 *  Created on: Jul 14, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_GST_PROPERTIES_MODULE_H_
#define SRC_GST_DEBUGGER_GST_PROPERTIES_MODULE_H_

#include "controller/iview.h"
#include "gvalue-converter/gvalue_base.h"

#include "protocol/gstdebugger.pb.h"

#include <gtkmm.h>

class GstPropertiesModule : public IBaseView
{
private:
	Gtk::Button *show_propetries_button;
	Gtk::Box *properties_box;
	Gtk::Entry *element_path_property_entry;

	void showPropertiesButton_clicked_cb();

	std::string previous_element_path;
	std::vector<Gtk::Box*> property_widgets;

	void append_property(const std::shared_ptr<GValueBase>& value_base, Property *property);
	bool update_property(const std::shared_ptr<GValueBase>& value_base, Property *property);
	void request_property(const std::string &property_name);

	void new_property(const Property& property);
	void new_property_();

public:
	GstPropertiesModule(const Glib::RefPtr<Gtk::Builder>& builder);

	void set_controller(const std::shared_ptr<Controller> &controller) override;
};

#endif /* SRC_GST_DEBUGGER_GST_PROPERTIES_MODULE_H_ */
