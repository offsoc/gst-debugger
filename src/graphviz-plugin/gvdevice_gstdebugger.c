/*
 * gvdevice_gstdebugger.c
 *
 *  Created on: Aug 14, 2015
 *      Author: loganek
 */

#include "graphviz-gstdebugger.h"

#include <gvplugin_device.h>
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

GtkWidget* drawing_area;

void set_drawing_area(GtkWidget *area)
{
	drawing_area = area;
}

static void gstdebugger_initialize(GVJ_t *firstjob)
{
	Display *dpy;
	const char *display_name = NULL;
	int scr;
	gtk_init (NULL, NULL);
	dpy = XOpenDisplay(display_name);
	if (dpy == NULL) {
		fprintf(stderr, "Failed to open XLIB display: %s\n",
				XDisplayName(NULL));
		return;
	}
	scr = DefaultScreen(dpy);
	firstjob->device_dpi.x = DisplayWidth(dpy, scr) * 25.4 / DisplayWidthMM(dpy, scr);
	firstjob->device_dpi.y = DisplayHeight(dpy, scr) * 25.4 / DisplayHeightMM(dpy, scr);
	firstjob->device_sets_dpi = TRUE;
}

static void gstdebugger_finalize(GVJ_t *firstjob)
{
	GVJ_t *job;

	for (job = firstjob; job; job = job->next_active) {
		g_object_set_data(G_OBJECT(drawing_area), "job", (gpointer) job);

		gtk_widget_show (drawing_area);
	}

	gtk_main();
}

static gvdevice_features_t device_features_gstdebugger = {
		GVDEVICE_DOES_TRUECOLOR
		| GVDEVICE_EVENTS,      /* flags */
		{0.,0.},                    /* default margin - points */
		{0.,0.},                    /* default page width, height - points */
		{96.,96.},                  /* dpi */
};

static gvdevice_engine_t device_engine_gstdebugger = {
		gstdebugger_initialize,
		NULL,			/* gtk_format */
		gstdebugger_finalize,
};

gvplugin_installed_t gvdevice_types_gstdebugger[] = {
		{0, "gstdebugger:cairo", 0, &device_engine_gstdebugger, &device_features_gstdebugger},
		{0, NULL, 0, NULL, NULL}
};

