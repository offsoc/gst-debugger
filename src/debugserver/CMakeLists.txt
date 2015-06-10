add_library(gstdebugserver SHARED gstdebugserver.c gstdebugservertcp.c)

include_directories(${GSTREAMER_INCLUDE_DIRS})

target_link_libraries(gstdebugserver gst-debugger-protocol-c ${GIO_LIBRARIES} ${GSTREAMER_LIBRARIES})
