lib_LTLIBRARIES = libgstdebugserver.la
libgstdebugserver_la_SOURCES = \
	gstdebugserver.c gstdebugserver.h \
	gstdebugservertcp.c gstdebugservertcp.h \
	gstdebugserverbuffer.c gstdebugserverbuffer.h \
	gstdebugservermessage.c gstdebugservermessage.h \
	gstdebugserverlog.c gstdebugserverlog.h \
	gstdebugserverqe.c gstdebugserverqe.h \
	gstdebugservertopology.c gstdebugservertopology.h \
	gstdebugserverfactory.c gstdebugserverfactory.h

libgstdebugserver_la_LDFLAGS  = $(GSTREAMER_LIBS) $(PROTOBUF_C_LIBS) $(GIO_LIBS)
libgstdebugserver_la_LIBADD = ../protocol/libprotocol-c.la ../utils/libutils.la
libgstdebugserver_la_CFLAGS = -I.. $(GSTREAMER_CFLAGS)
