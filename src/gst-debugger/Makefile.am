gst-debugger-resources.c: ui/gst-debugger-dialogs.glade ui/gst-debugger.glade ui/gst-debugger-logo.png 
	$(GLIB_COMPILE_RESOURCES) --generate-source $(srcdir)/gst-debugger-resources.xml --target=$@
gst-debugger-resources.h:
	$(GLIB_COMPILE_RESOURCES) --generate-header $(srcdir)/gst-debugger-resources.xml --target=$@

MOSTLYCLEANFILES = gst-debugger-resources.c gst-debugger-resources.h

EXTRA_DIST = 				\
	ui/gst-debugger-dialogs.glade	\
	ui/gst-debugger.glade		\
	ui/gst-debugger-logo.png

BUILT_SOURCES = gst-debugger-resources.h

bin_PROGRAMS = gst-debugger-@GST_API_VERSION@

gst_debugger_@GST_API_VERSION@_SOURCES = 		\
	main.cpp \
	main_window.cpp \
	main_window.h \
	controller/controller.cpp \
	controller/controller.h \
	controller/command_factory.cpp \
	controller/command_factory.h \
	ui_utils.cpp \
	ui_utils.h \
	gst-debugger-resources.c \
	gst-debugger-resources.h \
	controller/base_controller.h \
	controller/connection_controller.h \
	controller/tcp_client.cpp \
	controller/tcp_client.h \
	dialogs/connection_properties_dialog.cpp \
	dialogs/connection_properties_dialog.h \
	dialogs/remote_data_dialog.h \
	modules/control_module.h \
	modules/base_main_module.cpp \
	modules/base_main_module.h \
	modules/log_module.cpp \
	modules/log_module.h \
	modules/main_module.cpp \
	modules/main_module.h \
	controller/iview.h \
	controller/topology_controller.cpp \
	controller/topology_controller.h \
	common_model_columns.h \
	modules/message_module.cpp \
	modules/message_module.h \
	models/gst_enum_model.h \
	models/gst_factory_model.cpp \
	models/gst_factory_model.h \
	models/gst_pipeline_model.cpp \
	models/gst_pipeline_model.h \
	modules/event_module.cpp \
	modules/event_module.h \
	modules/query_module.cpp \
	modules/query_module.h \
	pipeline-drawer/graph_module.cpp \
	pipeline-drawer/graph_module.h \
	pipeline-drawer/gst_bin_to_dot_converter.cpp \
	pipeline-drawer/gst_bin_to_dot_converter.h \
	graphviz-plugin/graphviz-gstdebugger.h \
	graphviz-plugin/gvdevice_gstdebugger.c \
	graphviz-plugin/gvplugin_gstdebugger.c \
	controller/element_path_processor.h \
	modules/qe_control_module.h \
	dialogs/enums_dialog.cpp \
	dialogs/enums_dialog.h \
	dialogs/buffer_data_dialog.cpp \
	dialogs/buffer_data_dialog.h \
	dialogs/factories_dialog.cpp \
	dialogs/factories_dialog.h \
	modules/gst_properties_module.cpp \
	modules/gst_properties_module.h \
	modules/buffer_module.cpp \
	modules/buffer_module.h \
	models/gst_klass_model.h \
	dialogs/klasses_dialog.h \
	dialogs/klasses_dialog.cpp \
	gvalue-converter/gvalue_base.cpp \
	gvalue-converter/gvalue_base.h \
	gvalue-converter/gvalue_unknown.cpp \
	gvalue-converter/gvalue_unknown.h \
	gvalue-converter/gvalue_numeric.cpp \
	gvalue-converter/gvalue_numeric.h \
	gvalue-converter/gvalue_boolean.cpp \
	gvalue-converter/gvalue_boolean.h \
	gvalue-converter/gvalue_caps.cpp \
	gvalue-converter/gvalue_caps.h \
	gvalue-converter/gvalue_string.cpp \
	gvalue-converter/gvalue_string.h \
	gvalue-converter/gvalue_enum.cpp \
	gvalue-converter/gvalue_enum.h \
	gvalue-converter/gvalue_flags.cpp \
	gvalue-converter/gvalue_flags.h


gst_debugger_@GST_API_VERSION@_LDFLAGS = $(GTKMM_LIBS) $(GSTMM_LIBS) $(GVC_LIBS) $(PROTOBUF_LIBS) -lX11
gst_debugger_@GST_API_VERSION@_LDADD = ../common/libgst-debugger-common-cpp-@GST_DEBUGGER_API_VERSION@.la
gst_debugger_@GST_API_VERSION@_CXXFLAGS = $(GTKMM_CFLAGS) $(GSTMM_CFLAGS) $(GVC_CFLAGS) -DGST_DEBUGGER_LOCALEDIR=\"${GST_DEBUGGER_LOCALEDIR}\"
gst_debugger_@GST_API_VERSION@_CPPFLAGS = $(GVC_CFLAGS) -I$(top_srcdir)/src/
gst_debugger_@GST_API_VERSION@_CFLAGS = $(GIO_CFLAGS) $(GTK_CFLAGS)
