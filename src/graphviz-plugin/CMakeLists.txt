add_library(gvplugin_gstdebugger STATIC gvdevice_gstdebugger.c gvplugin_gstdebugger.c)

target_link_libraries(gvplugin_gstdebugger
	${GTK_LIBRARIES}
	-lX11
 )