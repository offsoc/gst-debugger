/*
 * gvplugin_gstdebugger.c
 *
 *  Created on: Aug 14, 2015
 *      Author: loganek
 */

#include <gvplugin.h>

extern gvplugin_installed_t gvdevice_types_gstdebugger[];

static gvplugin_api_t apis[] = {
		{ API_device, gvdevice_types_gstdebugger },
		{ (api_t)0, 0 },
};

gvplugin_library_t gvplugin_gstdebugger_LTX_library = { "gstdebugger", apis };


