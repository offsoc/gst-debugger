/*
 * addins_controller.cpp
 *
 *  Created on: Oct 14, 2015
 *      Author: loganek
 */

#include "addins_controller.h"

#include <boost/filesystem.hpp>

void AddinsController::load_from_dir(const std::string &path)
{
	namespace fs = boost::filesystem;
	fs::path someDir(path);
	fs::directory_iterator end_iter;

	if (!fs::exists(someDir) || !fs::is_directory(someDir))
		return;

	for( fs::directory_iterator dir_iter(someDir) ; dir_iter != end_iter ; ++dir_iter)
	{
		auto filename = dir_iter->path().generic_string();

		if (fs::is_regular_file(dir_iter->status()) && g_str_has_suffix (filename.c_str(), G_MODULE_SUFFIX))
		{
			load_module(filename);
		}
	}
}

void AddinsController::load_module(const std::string &filename)
{
	GstDebugger::Addin* (*create_addin)(GstDebugger::AddinsNotifier* notifier) = nullptr;
	auto module = std::make_shared<Glib::Module>(filename);
	if (!*module) return; // todo notify
	bool ok = module->get_symbol("gst_debugger_addin_init", (void *&)create_addin);
	if (!ok) return; // todo notify
	AddinInfo nfo; nfo.module = module; nfo.addin = std::shared_ptr<GstDebugger::Addin>(create_addin(this));
	addins[filename] = nfo;
}

void AddinsController::reload_addins()
{
	bool found;
	auto values = Glib::getenv("GST_DEBUGGER_PLUGIN_PATH", found);
	if (!found) return;

	gchar** paths = g_strsplit(values.c_str(), ";", -1);
	int i = 0;
	while (paths[i] != nullptr)
	{
		load_from_dir(paths[i++]);
	}
	g_strfreev(paths);

	on_addins_reloaded();
}

std::vector<std::shared_ptr<GstDebugger::Addin>> AddinsController::get_addins() const
{
	std::vector<std::shared_ptr<GstDebugger::Addin>> a;
	for (auto addin : addins)
		a.push_back(addin.second.addin);
	return a;
}
