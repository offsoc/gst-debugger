add_library(gstdebugserver SHARED gstdebugserver.c)

include_directories(${GSTREAMER_INCLUDE_DIRS})
link_directories(${GIO_LIBRARY_DIRS}) 

target_link_libraries(gstdebugserver ${GIO_LIBRARIES} ${GSTREAMER_LIBRARIES})
