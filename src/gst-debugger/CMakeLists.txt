add_custom_command(
	OUTPUT gst-debugger.ui.h
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ui/gst-debugger.glade gst-debugger.glade
	COMMAND xxd -i gst-debugger.glade gst-debugger.ui.h
	DEPENDS ui/gst-debugger.glade
)

add_executable(gst-debugger main.cpp main_window.cpp connection_properties_dialog.cpp gst-debugger.ui.h)

target_link_libraries(gst-debugger ${GTKMM_LIBRARIES})