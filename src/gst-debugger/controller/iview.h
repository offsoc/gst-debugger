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
protected:
	std::shared_ptr<Controller> controller;

public:
	virtual ~IBaseView() {}

	void set_controller(const std::shared_ptr<Controller> &controller) { this->controller = controller; }
};

class IMainView : public Gtk::Window, public IBaseView
{
public:
	IMainView(BaseObjectType* cobject) : Gtk::Window(cobject) {}
	virtual ~IMainView() {}

	virtual void set_current_model(const std::shared_ptr<ElementModel> &model) = 0;

	virtual void set_controller(const std::shared_ptr<Controller> &controller) = 0;

	virtual void set_debug_categories(const std::vector<std::string> &debug_categories) = 0;
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_IVIEW_H_ */
