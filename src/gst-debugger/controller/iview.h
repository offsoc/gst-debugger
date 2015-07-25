/*
 * iview.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_IVIEW_H_
#define SRC_GST_DEBUGGER_CONTROLLER_IVIEW_H_

#include "models/gst_pipeline_model.h"

#include <gtkmm/window.h>

#include <memory>

class Controller;

class IBaseView
{
	std::map<std::string, Glib::Dispatcher> dispatchers;

protected:
	std::shared_ptr<Controller> controller;

	void create_dispatcher(const std::string &dispatcher_name, const sigc::slot<void>& slot)
	{
		dispatchers[dispatcher_name].connect(slot);
	}

	void gui_emit(const std::string &dispatcher_name)
	{
		dispatchers[dispatcher_name].emit();
	}

public:
	virtual ~IBaseView() {}

	virtual void set_controller(const std::shared_ptr<Controller> &controller) { this->controller = controller; }
};

class IMainView : public Gtk::Window, public IBaseView
{
public:
	IMainView(BaseObjectType* cobject) : Gtk::Window(cobject) {}
	virtual ~IMainView() {}

	virtual void set_current_model(const std::shared_ptr<ElementModel> &model) = 0;

	virtual void set_controller(const std::shared_ptr<Controller> &controller) = 0;
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_IVIEW_H_ */
