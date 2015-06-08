PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders gstdebugger.proto)
add_library(gst-debugger-protocol STATIC ${ProtoSources} ${ProtoHeaders})
target_link_libraries(gst-debugger-protocol ${PROTOBUF_LIBRARY})