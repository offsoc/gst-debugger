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

gst_debugger_headers =					\
	common_model_columns.h				\
	controller/base_controller.h			\
	controller/command_factory.h			\
	controller/connection_controller.h		\
	controller/controller.h				\
	controller/element_path_processor.h		\
	controller/iview.h				\
	controller/tcp_client.h				\
	controller/topology_controller.h		\
	dialogs/buffer_data_dialog.h			\
	dialogs/connection_properties_dialog.h		\
	dialogs/enums_dialog.h				\
	dialogs/factories_dialog.h			\
	dialogs/remote_data_dialog.h			\
	graphviz-plugin/graphviz-gstdebugger.h		\
	gst-debugger-resources.h			\
	gvalue-converter/gvalue_base.h			\
	gvalue-converter/gvalue_boolean.h		\
	gvalue-converter/gvalue_caps.h			\
	gvalue-converter/gvalue_enum.h			\
	gvalue-converter/gvalue_numeric.h		\
	gvalue-converter/gvalue_string.h		\
	main_window.h					\
	models/gst_enum_model.h				\
	models/gst_factory_model.h			\
	models/gst_pipeline_model.h			\
	models/remote_data_container.h			\
	modules/base_main_module.h			\
	modules/bus_messages_module.h			\
	modules/control_module.h			\
	modules/gst_log_module.h			\
	modules/gst_properties_module.h			\
	modules/log_module.h				\
	modules/main_module.h				\
	modules/pad_path_control_module.h		\
	modules/pad_path_types_control_module.h		\
	modules/types_control_modules.h			\
	pipeline-drawer/graph_module.h			\
	pipeline-drawer/gst_bin_to_dot_converter.h	\
	ui_utils.h					


gst_debugger_@GST_API_VERSION@_SOURCES = 		\
	controller/command_factory.cpp			\
	controller/controller.cpp			\
	controller/tcp_client.cpp			\
	controller/topology_controller.cpp		\
	dialogs/buffer_data_dialog.cpp			\
	dialogs/connection_properties_dialog.cpp	\
	dialogs/enums_dialog.cpp			\
	dialogs/factories_dialog.cpp			\
	graphviz-plugin/gvdevice_gstdebugger.c		\
	graphviz-plugin/gvplugin_gstdebugger.c		\
	gst-debugger-resources.c			\
	gvalue-converter/gvalue_base.cpp		\
	gvalue-converter/gvalue_boolean.cpp		\
	gvalue-converter/gvalue_caps.cpp		\
	gvalue-converter/gvalue_enum.cpp		\
	gvalue-converter/gvalue_numeric.cpp		\
	gvalue-converter/gvalue_string.cpp		\
	main.cpp					\
	main_window.cpp					\
	models/gst_factory_model.cpp			\
	models/gst_pipeline_model.cpp			\
	modules/base_main_module.cpp			\
	modules/bus_messages_module.cpp			\
	modules/gst_log_module.cpp			\
	modules/gst_properties_module.cpp		\
	modules/log_module.cpp				\
	modules/main_module.cpp				\
	modules/pad_data_modules.cpp			\
	modules/pad_path_control_module.cpp		\
	modules/types_control_module.cpp		\
	pipeline-drawer/graph_module.cpp		\
	pipeline-drawer/gst_bin_to_dot_converter.cpp	\
	ui_utils.cpp					\
	$(gst_debugger_headers)

gst_debugger_@GST_API_VERSION@_LDFLAGS = $(GTKMM_LIBS) $(GSTMM_LIBS) $(GVC_LIBS) $(PROTOBUF_LIBS) -lX11
gst_debugger_@GST_API_VERSION@_LDADD = ../common/libgst-debugger-common-cpp-@GST_DEBUGGER_API_VERSION@.la
gst_debugger_@GST_API_VERSION@_CXXFLAGS = $(GTKMM_CFLAGS) $(GSTMM_CFLAGS) $(GVC_CFLAGS)
gst_debugger_@GST_API_VERSION@_CPPFLAGS = $(GVC_CFLAGS) -I$(top_srcdir)/src/
gst_debugger_@GST_API_VERSION@_CFLAGS = $(GIO_CFLAGS) $(GTK_CFLAGS)
