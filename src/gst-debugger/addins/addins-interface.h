/*
 * addins-interface.h
 *
 *  Created on: Oct 13, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_ADDINS_ADDINS_INTERFACE_H_
#define SRC_GST_DEBUGGER_ADDINS_ADDINS_INTERFACE_H_

#include <addins/addins-notifier.h>

#include <gtkmm/widget.h>

#include <string>

namespace GstDebugger {

class Addin
{
	std::string name;

protected:
	Gtk::Widget *widget = nullptr;

	virtual void create_widget() = 0;

public:
	Addin(const std::string &name, AddinsNotifier* notifier) : name(name) {}
	virtual ~Addin() { delete widget; }

	Gtk::Widget *get_widget()
	{
		if (widget == nullptr)
			create_widget();
		return widget;
	}

	std::string get_name() const { return name; }
};

}

#endif /* SRC_GST_DEBUGGER_ADDINS_ADDINS_INTERFACE_H_ */
