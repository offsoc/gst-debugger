AC_INIT([gst-debugger], [0.1.0], 
		[https://github.com/loganek/gst-debugger/issues])
AC_PREREQ([2.59])

AC_CONFIG_MACRO_DIR([build])

PKG_CHECK_MODULES([GIO], [gio-2.0 >= 2.45.4])
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.16])
PKG_CHECK_MODULES([GSTREAMER], [gstreamer-1.0 >= 1.4.5])
PKG_CHECK_MODULES(PROTOBUF_C, libprotobuf-c >= 1.1.1)
PKG_CHECK_MODULES(PROTOBUF, protobuf >= 2.6.1)
PKG_CHECK_MODULES(CGRAPH, libcgraph >= 2.38)
PKG_CHECK_MODULES(GVC, libgvc >= 2.38)
PKG_CHECK_MODULES(GTKMM, gtkmm-3.0 >= 3.15.4)
PKG_CHECK_MODULES(GSTMM, gstreamermm-1.0 >= 1.4.3)

AM_INIT_AUTOMAKE([check-news])

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])

LT_PREREQ
LT_INIT

AC_CONFIG_FILES([
	Makefile 
	src/Makefile
	src/debugserver/Makefile
	src/graphviz-plugin/Makefile
	src/gst-debugger/Makefile
	src/protocol/Makefile
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
