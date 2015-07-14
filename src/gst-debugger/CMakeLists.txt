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
	connection_properties_dialog.cpp
	gst_debugger_tcp_client.cpp
	gst_buffer_module.cpp
	gst_message_module.cpp
	gst_log_module.cpp
	gst_event_module.cpp
	gst_properties_module.cpp
	gst_query_module.cpp
	gst_qe_module.cpp
	gst-debugger.ui.h
	pipeline-drawer/gst_bin_to_dot_converter.cpp
	pipeline-drawer/graph_module.cpp
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