/*
 * controller.h
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_

#include "models/gst_pipeline_model.h"

#include "iview.h"

class Controller : public std::enable_shared_from_this<Controller>
{
private:
	std::shared_ptr<ElementModel> current_model;
	std::shared_ptr<IView> view;

public:
	Controller();
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_CONTROLLER_H_ */
