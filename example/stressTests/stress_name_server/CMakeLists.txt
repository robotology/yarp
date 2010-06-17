cmake_minimum_required(VERSION 2.6)

find_package(YARP REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})

link_libraries(${YARP_LIBRARIES})

add_executable(stress_name_server stress_name_server.cpp)
