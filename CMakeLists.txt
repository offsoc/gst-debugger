cmake_minimum_required(VERSION 2.8.9)

project(gst-debugger)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpedantic -std=c11 -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wno-unused-parameter")

SET(PROJECT_SOURCE_DIR "${gst-debugger_SOURCE_DIR}/src")

include_directories(${PROJECT_SOURCE_DIR})

INCLUDE(FindPkgConfig)

pkg_check_modules(LIBPB_C REQUIRED libprotobuf-c>=1.1.1)

pkg_check_modules(GIOMM REQUIRED giomm-2.4)
include_directories(${GIOMM_INCLUDE_DIRS})
link_directories(${GIOMM_LIBRARY_DIRS})

pkg_check_modules(GSTMM REQUIRED gstreamermm-1.0)
include_directories(${GSTMM_INCLUDE_DIRS})
link_directories(${GSTMM_LIBRARY_DIRS})

pkg_check_modules (GTKMM REQUIRED gtkmm-3.0>=3.12)
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS}) 

pkg_check_modules(CGRAPH REQUIRED libcgraph)
include_directories(${CGRAPH_INCLUDE_DIRS})
link_directories(${CGRAPH_LIBRARY_DIRS}) 

pkg_check_modules(GVC REQUIRED libgvc)
include_directories(${GVC_INCLUDE_DIRS})
link_directories(${GVC_LIBRARY_DIRS}) 

pkg_check_modules(GIO REQUIRED gio-2.0)
link_directories(${GIO_LIBRARY_DIRS}) 

pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
include_directories(${GSTREAMER_INCLUDE_DIRS})

# it only checks version
pkg_check_modules(PROTOBUFF REQUIRED protobuf>=2.6.1)
find_package(Protobuf REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)
