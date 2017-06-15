cmake_minimum_required(VERSION 3.0)

project(log_test)

find_package(YARP REQUIRED)

add_executable(log_test log_test.cpp)
target_link_libraries(log_test YARP::YARP_OS
                               YARP::YARP_init)
