plugin_LTLIBRARIES = libgstdebugserver.la

libgstdebugserver_la_SOURCES = \
	gstdebugserver.c gstdebugserver.h \
	gstdebugservertcp.c gstdebugservertcp.h \
	gstdebugserverlog.c gstdebugserverlog.h \
	gstdebugservermessage.c gstdebugservermessage.h \
	gstdebugservertypes.c gstdebugservertypes.h \
	gstdebugserverqe.c gstdebugserverqe.h \
	gstdebugservertopology.c gstdebugservertopology.h \
	gstdebugserverwatcher.c gstdebugserverwatcher.h \
	gstdebugserverbuffer.c gstdebugserverbuffer.h
#	gstdebugserverfactory.c gstdebugserverfactory.h

libgstdebugserver_la_LDFLAGS  = $(GSTREAMER_LIBS) $(PROTOBUF_C_LIBS) $(GIO_LIBS)
libgstdebugserver_la_LIBADD = ../common/libgst-debugger-common-c-@GST_DEBUGGER_API_VERSION@.la
libgstdebugserver_la_CFLAGS = $(GSTREAMER_CFLAGS) -I$(top_srcdir)/src/
