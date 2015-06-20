find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS}
		${PROJECT_SOURCE_DIR}/src)

add_executable(test main.cpp serializer-test.cpp)
target_link_libraries(test ${GTEST_BOTH_LIBRARIES} pthread gst-debugger-protocol-cpp ${GIOMM_LIBRARIES})
