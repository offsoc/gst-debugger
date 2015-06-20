cmake_minimum_required(VERSION 2.8.9)

project(gst-debugger)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wno-unused-parameter")

SET(PROJECT_SOURCE_DIR "${gst-debugger_SOURCE_DIR}/src")

include_directories(${PROJECT_SOURCE_DIR})

INCLUDE(FindPkgConfig)

pkg_check_modules(LIBPB_C REQUIRED libprotobuf-c)

pkg_check_modules(GIOMM REQUIRED giomm-2.4)
include_directories(${GIOMM_INCLUDE_DIRS})
link_directories(${GIOMM_LIBRARY_DIRS}) 

pkg_check_modules(GIO REQUIRED gio-2.0)
link_directories(${GIO_LIBRARY_DIRS}) 

pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
include_directories(${GSTREAMER_INCLUDE_DIRS})

find_package(Protobuf REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)
