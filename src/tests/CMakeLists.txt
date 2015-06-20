add_executable(serializer-test serializer-test.cpp)

target_link_libraries(serializer-test gst-debugger-protocol-cpp  ${GSTREAMER_LIBRARIES} ${GIOMM_LIBRARIES})
