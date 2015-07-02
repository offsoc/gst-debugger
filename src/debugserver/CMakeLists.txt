add_library(gstdebugserver SHARED 
	gstdebugserver.c
	gstdebugservertcp.c
	gstdebugservermessage.c
	gstdebugserverlog.c
	gstdebugserverqe.c
	gstdebugserverutils.c
)

target_link_libraries(gstdebugserver gst-debugger-protocol-c ${GIO_LIBRARIES} ${GSTREAMER_LIBRARIES})
