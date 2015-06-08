cmake_minimum_required(VERSION 2.8.9)

project(gst-debugger)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed")
SET(PROJECT_SOURCE_DIR "${gst-debugger_SOURCE_DIR}/src")

INCLUDE(FindPkgConfig)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)
