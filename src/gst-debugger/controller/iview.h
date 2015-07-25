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
	struct QueuedDispacher
	{
		Glib::Dispatcher dispatcher;
		GAsyncQueue *queue;
	};
	std::map<std::string, QueuedDispacher> dispatchers;

protected:
	std::shared_ptr<Controller> controller;

	void create_dispatcher(const std::string &dispatcher_name, const sigc::slot<void>& slot, GDestroyNotify queue_free_method)
	{
		dispatchers[dispatcher_name].dispatcher.connect(slot);
		dispatchers[dispatcher_name].queue = g_async_queue_new_full (queue_free_method);
	}

	void gui_emit(const std::string &dispatcher_name)
	{
		dispatchers[dispatcher_name].dispatcher.emit();
	}

	void gui_push(const std::string &dispatcher_name, gpointer data)
	{
		g_async_queue_push (dispatchers[dispatcher_name].queue, data);
	}

	template<typename T>
	T gui_pop(const std::string &dispatcher_name)
	{
		return reinterpret_cast<T>(g_async_queue_pop (dispatchers[dispatcher_name].queue));
	}

	template<typename T>
	T gui_try_pop(const std::string &dispatcher_name)
	{
		return reinterpret_cast<T>(g_async_queue_try_pop (dispatchers[dispatcher_name].queue));
	}

public:
	virtual ~IBaseView()
	{
		//for (auto d : dispatchers)
		{
		//	g_async_queue_unref (d.second.queue);
		}
	}

	virtual void set_controller(const std::shared_ptr<Controller> &controller) { this->controller = controller; }
};

class IMainView : public Gtk::Window, public IBaseView
{
public:
	IMainView(BaseObjectType* cobject) : Gtk::Window(cobject) {}
	virtual ~IMainView() {}

	virtual void set_controller(const std::shared_ptr<Controller> &controller) = 0;
};

#endif /* SRC_GST_DEBUGGER_CONTROLLER_IVIEW_H_ */
