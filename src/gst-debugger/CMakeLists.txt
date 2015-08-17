add_subdirectory(gvalue-converter)

add_custom_command(
	OUTPUT gst-debugger-resources.c
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/ui/ ./ui
	COMMAND glib-compile-resources --generate-source ${CMAKE_CURRENT_SOURCE_DIR}/gst-debugger-resources.xml --target=gst-debugger-resources.c
	COMMAND glib-compile-resources --generate-header ${CMAKE_CURRENT_SOURCE_DIR}/gst-debugger-resources.xml --target=gst-debugger-resources.h
	DEPENDS 
		ui/gst-debugger-dialogs.glade 
		ui/gst-debugger.glade 
		ui/gst-debugger-logo.png 
		gst-debugger-resources.xml
)

add_executable(gst-debugger
	main.cpp
	main_window.cpp
	gst-debugger-resources.h
	gst-debugger-resources.c
	ui_utils.cpp
	
	dialogs/connection_properties_dialog.cpp
	dialogs/enums_dialog.cpp
	dialogs/factories_dialog.cpp
	dialogs/buffer_data_dialog.cpp

	modules/gst_buffer_module.cpp
	modules/gst_message_module.cpp
	modules/gst_log_module.cpp
	modules/gst_event_module.cpp
	modules/gst_properties_module.cpp
	modules/gst_query_module.cpp
	modules/gst_qe_module.cpp

	pipeline-drawer/gst_bin_to_dot_converter.cpp
	pipeline-drawer/graph_module.cpp

	controller/command_factory.cpp
	controller/controller.cpp
	controller/tcp_client.cpp
	controller/topology_controller.cpp

	models/gst_pipeline_model.cpp
	models/gst_factory_model.cpp
)

target_link_libraries(gst-debugger
	gst-debugger-protocol-cpp
	gvplugin_gstdebugger
	gvalue-converter
	utils
	${GIOMM_LIBRARIES}
	${GTKMM_LIBRARIES}
	${GSTMM_LIBRARIES}
	${GVC_LIBRARIES}
	${CGRAPH_LIBRARIES}
	-pthread
)