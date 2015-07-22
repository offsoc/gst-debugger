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

class IView : public Gtk::Window
{
protected:
	std::shared_ptr<Controller> controller;

public:
	IView(BaseObjectType* cobject) : Gtk::Window(cobject) {}
	virtual ~IView() {}

	void set_controller(const std::shared_ptr<Controller> &controller) { this->controller = controller; }

	virtual void set_current_model(const std::shared_ptr<ElementModel> &model) = 0;
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_IVIEW_H_ */
