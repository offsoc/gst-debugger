%.pb-c.c %.pb-c.h: %.proto
	$(PROTOC_C) -I$(srcdir) --c_out=$(builddir) $^

%.pb.cc %.pb.h: %.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(builddir) $^

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgst-debugger-common.pc

dist_noinst_DATA = gstdebugger.proto

MOSTLYCLEANFILES = gstdebugger.pb-c.c gstdebugger.pb-c.h gstdebugger.pb.cc gstdebugger.pb.h

common_sources =		\
	deserializer.c		\
	gst-utils.c		\
	protocol_utils.c	\
	serializer.c

common_headers =		\
	buffer-prepare-utils.h	\
	common.h		\
	deserializer.h		\
	gst-utils.h		\
	protocol_utils.h	\
	serializer.h

common_include_dir = $(includedir)/gst-debugger/gst-debugger

lib_LTLIBRARIES = libgst-debugger-common-c.la libgst-debugger-common-cpp.la 

libgst_debugger_common_c_la_SOURCES = $(common_sources) gstdebugger.pb-c.c
libgst_debugger_common_c_lainclude_HEADERS = $(common_headers) gstdebugger.pb-c.h
libgst_debugger_common_c_laincludedir = $(common_include_dir)
libgst_debugger_common_c_la_LDFLAGS = $(PROTOBUF_C_LIBS) $(GSTREAMER_LIBS)
libgst_debugger_common_c_la_CFLAGS = -I.. $(PROTOBUF_C_CFLAGS) $(GSTREAMER_CFLAGS)

libgst_debugger_common_cpp_la_SOURCES = $(common_sources) gstdebugger.pb.cc
libgst_debugger_common_cpp_lainclude_HEADERS = $(common_headers) gstdebugger.pb.h
libgst_debugger_common_cpp_laincludedir = $(common_include_dir)
libgst_debugger_common_cpp_la_LDFLAGS  = $(PROTOBUF_LIBS) $(GSTREAMER_LIBS)
libgst_debugger_common_cpp_la_CFLAGS = -I.. $(PROTOBUF_CFLAGS) $(GSTREAMER_CFLAGS)
