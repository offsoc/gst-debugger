%.pb-c.c %.pb-c.h: %.proto
	$(PROTOC_C) -I$(srcdir) --c_out=$(builddir) $^

%.pb.cc %.pb.h: %.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(builddir) $^

dist_noinst_DATA = gstdebugger.proto

MOSTLYCLEANFILES = gstdebugger.pb-c.c gstdebugger.pb-c.h gstdebugger.pb.cc gstdebugger.pb.h
BUILT_SOURCES = gstdebugger.pb-c.h gstdebugger.pb.h

COMMON_SOURCES = buffer-prepare-utils.h gst-utils.c gst-utils.h protocol_utils.c serializer.c deserializer.c

lib_LTLIBRARIES = libprotocol-c.la libprotocol-cpp.la 

libprotocol_c_la_SOURCES = $(COMMON_SOURCES) gstdebugger.pb-c.c gstdebugger.pb-c.h
libprotocol_c_la_LDFLAGS = $(PROTOBUF_C_LIBS) $(GSTREAMER_LIBS)
libprotocol_c_la_CFLAGS = -I.. $(PROTOBUF_C_CFLAGS) $(GSTREAMER_CFLAGS)

libprotocol_cpp_la_SOURCES = $(COMMON_SOURCES) gstdebugger.pb.cc gstdebugger.pb.h
libprotocol_cpp_la_LDFLAGS  = $(PROTOBUF_LIBS) $(GSTREAMER_LIBS)
libprotocol_cpp_la_CFLAGS = -I.. $(PROTOBUF_CFLAGS) $(GSTREAMER_CFLAGS)
