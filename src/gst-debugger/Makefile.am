gst-debugger-resources.c: ui/gst-debugger-dialogs.glade ui/gst-debugger.glade ui/gst-debugger-logo.png 
	$(GLIB_COMPILE_RESOURCES) --generate-source $(srcdir)/gst-debugger-resources.xml --target=$@
gst-debugger-resources.h:
	$(GLIB_COMPILE_RESOURCES) --generate-header $(srcdir)/gst-debugger-resources.xml --target=$@

dist_noinst_DATA = gst-debugger-resources.c gst-debugger-resources.h

MOSTLYCLEANFILES = gst-debugger-resources.c gst-debugger-resources.h
BUILT_SOURCES = gst-debugger-resources.h

bin_PROGRAMS = gst-debugger
gst_debugger_SOURCES = \
	main.cpp \
	main_window.cpp \
	gst-debugger-resources.h \
	gst-debugger-resources.c \
	ui_utils.cpp \
	dialogs/connection_properties_dialog.cpp \
	dialogs/enums_dialog.cpp \
	dialogs/factories_dialog.cpp \
	dialogs/buffer_data_dialog.cpp \
	modules/gst_buffer_module.cpp \
	modules/gst_message_module.cpp \
	modules/gst_log_module.cpp \
	modules/gst_event_module.cpp \
	modules/gst_properties_module.cpp \
	modules/gst_query_module.cpp \
	modules/gst_qe_module.cpp \
	pipeline-drawer/gst_bin_to_dot_converter.cpp \
	pipeline-drawer/graph_module.cpp \
	controller/command_factory.cpp \
	controller/controller.cpp \
	controller/tcp_client.cpp \
	controller/topology_controller.cpp \
	models/gst_pipeline_model.cpp \
	models/gst_factory_model.cpp \
	graphviz-plugin/gvdevice_gstdebugger.c \
	graphviz-plugin/gvplugin_gstdebugger.c \
	gvalue-converter/gvalue_base.cpp \
	gvalue-converter/gvalue_boolean.cpp \
	gvalue-converter/gvalue_caps.cpp \
	gvalue-converter/gvalue_enum.cpp \
	gvalue-converter/gvalue_numeric.cpp \
	gvalue-converter/gvalue_string.cpp

gst_debugger_LDFLAGS = $(GTKMM_LIBS) $(GSTMM_LIBS) $(GVC_LIBS) $(PROTOBUF_LIBS)
gst_debugger_LDADD = ../protocol/libprotocol-cpp.la -lX11
gst_debugger_CXXFLAGS = -I.. $(GTKMM_CFLAGS) $(GSTMM_CFLAGS) $(GVC_CFLAGS)
gst_debugger_CPPFLAGS = $(GVC_CFLAGS)
gst_debugger_CFLAGS = $(GIO_CFLAGS) $(GTK_CFLAGS)
	