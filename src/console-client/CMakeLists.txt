add_executable(console-client main.cpp)

target_link_libraries(console-client gst-debugger-protocol-cpp ${GIOMM_LIBRARIES})
