/*
 * addins_controller.h
 *
 *  Created on: Oct 13, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_CONTROLLER_ADDINS_CONTROLLER_H_
#define SRC_GST_DEBUGGER_CONTROLLER_ADDINS_CONTROLLER_H_

#include "addins/addins-interface.h"
#include "addins/addins-notifier.h"

#include <glibmm.h>

class AddinsController : public GstDebugger::AddinsNotifier
{
	struct AddinInfo
	{
		std::shared_ptr<Glib::Module> module;
		std::shared_ptr<GstDebugger::Addin> addin;
	};

	std::map<std::string, AddinInfo> addins;

	void load_from_dir(const std::string &path);

	void load_module(const std::string &filename);

public:
	virtual ~AddinsController() {}

	void reload_addins();

	std::vector<std::shared_ptr<GstDebugger::Addin>> get_addins() const;

	sigc::signal<void> on_addins_reloaded;
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_ADDINS_CONTROLLER_H_ */
