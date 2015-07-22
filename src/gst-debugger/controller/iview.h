/*
 * iview.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_IVIEW_H_
#define SRC_GST_DEBUGGER_CONTROLLER_IVIEW_H_

#include "models/gst_pipeline_model.h"

#include <memory>

class Controller;

class IView
{
protected:
	std::shared_ptr<Controller> controller;

public:
	IView(const std::shared_ptr<Controller> &controller) : controller(controller) {}
	virtual ~IView() {}

	virtual void set_current_model(const std::shared_ptr<ElementModel> &model) = 0;
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_IVIEW_H_ */
