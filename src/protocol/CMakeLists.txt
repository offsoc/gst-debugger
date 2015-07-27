PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders gstdebugger.proto)

set (PROTOCOL_FILES protocol_utils.c serializer.c deserializer.c)

add_library(gst-debugger-protocol-cpp STATIC ${PROTOCOL_FILES} ${ProtoSources} ${ProtoHeaders})
add_library(gst-debugger-protocol-c STATIC ${PROTOCOL_FILES} gstdebugger.pb-c.c gstdebugger.pb-c.h)
set_target_properties(gst-debugger-protocol-c PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(gst-debugger-protocol-cpp utils ${PROTOBUF_LIBRARY} ${GSTREAMER_LIBRARIES})

link_directories(${LIBPB_C_LIBRARY_DIRS}) 
target_link_libraries(gst-debugger-protocol-c utils ${LIBPB_C_LIBRARIES} ${GSTREAMER_LIBRARIES})

add_custom_command(OUTPUT gstdebugger.pb-c.c gstdebugger.pb-c.h 
	COMMAND protoc-c -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/gstdebugger.proto --c_out=${CMAKE_CURRENT_BINARY_DIR}
)