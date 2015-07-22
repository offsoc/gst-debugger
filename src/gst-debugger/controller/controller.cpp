/*
 * controller.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: loganek
 */

#include "controller.h"

Controller::Controller()
 : current_model(ElementModel::get_root())
{
	// todo view = std::make_shared<MainWindow>(shared_from_this());
}
