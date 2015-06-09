PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders gstdebugger.proto)

add_library(gst-debugger-protocol-cpp STATIC protocol_utils.c ${ProtoSources} ${ProtoHeaders})
add_library(gst-debugger-protocol-c STATIC protocol_utils.c gstdebugger.pb-c.c gstdebugger.pb-c.h)

target_link_libraries(gst-debugger-protocol-cpp ${PROTOBUF_LIBRARY})

add_custom_command(OUTPUT gstdebugger.pb-c.c gstdebugger.pb-c.h 
	COMMAND protoc-c -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/gstdebugger.proto --c_out=${CMAKE_CURRENT_BINARY_DIR}
)