add_library(gstdebugserver SHARED 
	gstdebugserver.c
	gstdebugservertcp.c
	gstdebugserverbuffer.c
	gstdebugservermessage.c
	gstdebugserverlog.c
	gstdebugserverqe.c
	gstdebugservertopology.c
)

target_link_libraries(gstdebugserver utils gst-debugger-protocol-c ${GIO_LIBRARIES} ${GSTREAMER_LIBRARIES})
