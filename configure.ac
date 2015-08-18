AC_INIT([gst-debugger], [0.1.0], 
		[https://github.com/loganek/gst-debugger/issues])

AC_PREREQ([2.59])

AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_HEADERS([src/config.h])

AC_SUBST([GST_DEBUGGER_API_VERSION], [0.1])

AC_SUBST([GLIB_VERSION], [2.45.4])
AC_SUBST([GTK_VERSION], [3.15.4])
AC_SUBST([GSTREAMER_VERSION], 1.4.3)
AC_SUBST([PROTOBUF_VERSION], [2.6.1])
AC_SUBST([PROTOBUF_C_VERSION], [1.1.1])
AC_SUBST([GVC_VERSION], [2.38])

PKG_CHECK_MODULES([GIO], [gio-2.0 >= "$GLIB_VERSION"])
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= $GTK_VERSION])
PKG_CHECK_MODULES(GTKMM, gtkmm-3.0 >= $GTK_VERSION)
PKG_CHECK_MODULES([GSTREAMER], [gstreamer-1.0 >= $GSTREAMER_VERSION])
PKG_CHECK_MODULES(GSTMM, gstreamermm-1.0 >= $GSTREAMER_VERSION)
PKG_CHECK_MODULES(PROTOBUF, protobuf >= PROTOBUF_VERSION)
PKG_CHECK_MODULES(PROTOBUF_C, libprotobuf-c >= PROTOBUF_C_VERSION)
PKG_CHECK_MODULES(GVC, libgvc >= GVC_VERSION)

AM_INIT_AUTOMAKE([1.11 check-news subdir-objects -Wall])
AM_SILENT_RULES([yes])
AM_PROG_AR

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])

LT_PREREQ
LT_INIT

AC_CONFIG_FILES([
	Makefile 
	src/Makefile
	src/debugserver/Makefile
	src/gst-debugger/Makefile
	src/common/libgst-debugger-common-${GST_DEBUGGER_API_VERSION}.pc:src/common/libgst-debugger-common.pc.in
	src/common/Makefile
])

AC_PROG_CC
AC_PROG_INSTALL

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

AC_CHECK_PROG([PROTOC], [protoc], [protoc])
AS_IF([test "x${PROTOC}" == "x"],
    [AC_MSG_ERROR([ProtoBuf compiler "protoc" not found.])])

AC_CHECK_PROG([PROTOC_C], [protoc-c], [protoc-c])
AS_IF([test "x${PROTOC_C}" == "x"],
    [AC_MSG_ERROR([ProtoBuf C compiler "protoc-c" not found.])])

AC_CHECK_PROG([GLIB_COMPILE_RESOURCES], [glib-compile-resources], [glib-compile-resources])
AS_IF([test "x${GLIB_COMPILE_RESOURCES}" == "x"],
    [AC_MSG_ERROR([GLIB resources compiler "glib-compile-resources" not found.])])

AC_OUTPUT
