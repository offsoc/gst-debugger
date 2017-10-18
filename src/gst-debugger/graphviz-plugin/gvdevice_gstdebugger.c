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

static GtkWidget* drawing_area;
GMainLoop *graphviz_gstdebugger_plugin_main_loop = NULL;

struct DisplayRef
{
    Display *dpy;
    int refcount;
};

static struct DisplayRef dpy_ref = { NULL, 0 };

void graphviz_gstdebugger_set_drawing_area(GtkWidget *area)
{
	drawing_area = area;
}

void graphviz_gstdebugger_set_main_loop(GMainLoop *loop)
{
	graphviz_gstdebugger_plugin_main_loop = loop;
}

static void gstdebugger_initialize(GVJ_t *firstjob)
{
	const char *display_name = NULL;
	int scr;
	gtk_init (NULL, NULL);
    if (dpy_ref.dpy == NULL)
    {
        dpy_ref.dpy = XOpenDisplay(display_name);
    }
    if (dpy_ref.dpy == NULL) {
		fprintf(stderr, "Failed to open XLIB display: %s\n",
				XDisplayName(NULL));
		return;
	}
    dpy_ref.refcount++;
    scr = DefaultScreen(dpy_ref.dpy);
    firstjob->device_dpi.x = DisplayWidth(dpy_ref.dpy, scr) * 25.4 / DisplayWidthMM(dpy_ref.dpy, scr);
    firstjob->device_dpi.y = DisplayHeight(dpy_ref.dpy, scr) * 25.4 / DisplayHeightMM(dpy_ref.dpy, scr);
	firstjob->device_sets_dpi = TRUE;
}

static void gstdebugger_finalize(GVJ_t *firstjob)
{
	GVJ_t *job;

	for (job = firstjob; job; job = job->next_active) {
		g_object_set_data(G_OBJECT(drawing_area), "job", (gpointer) job);

		gtk_widget_show (drawing_area);
	}

	g_main_loop_run (graphviz_gstdebugger_plugin_main_loop);

    if (dpy_ref.refcount > 0)
    {
        dpy_ref.refcount--;
        if (dpy_ref.refcount == 0 && dpy_ref.dpy != NULL)
        {
            XCloseDisplay(dpy_ref.dpy);
            dpy_ref.dpy = NULL;
        }
    }
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

