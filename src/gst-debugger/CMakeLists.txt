add_subdirectory(gvalue-converter)

add_custom_command(
	OUTPUT gst-debugger.ui.h
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ui/gst-debugger.glade gst-debugger.glade
	COMMAND xxd -i gst-debugger.glade gst-debugger.ui.h
	DEPENDS ui/gst-debugger.glade
)

add_executable(gst-debugger
	main.cpp
	main_window.cpp
	gst-debugger.ui.h
	
	dialogs/connection_properties_dialog.cpp
	dialogs/enums_dialog.cpp
	
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
	models/gst_enum_model.cpp
	models/gst_factory_model.cpp
)

target_link_libraries(gst-debugger
	gst-debugger-protocol-cpp
	gvalue-converter
	utils
	${GIOMM_LIBRARIES}
	${GTKMM_LIBRARIES}
	${GSTMM_LIBRARIES}
	${GVC_LIBRARIES}
	${CGRAPH_LIBRARIES}
	-L/usr/local/lib/graphviz -lgvplugin_gstdebugger #todo
	-pthread
)