cmake_minimum_required(VERSION 2.8.9)

project(log_test)

find_package(YARP REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})

add_executable(log_test log_test.cpp)
target_link_libraries(log_test YARP::YARP_OS)
