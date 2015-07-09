add_library(utils STATIC gst-utils.c)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

target_link_libraries(utils ${GSTREAMER_LIBRARIES})
