cmake_minimum_required(VERSION 2.4)

find_package(YARP REQUIRED)

project(yarpc)

include_directories(${PROJECT_SOURCE_DIR})

add_library(yarpc yarp.h yarp.cpp yarpnetwork.cpp yarpport.cpp yarpcontact.cpp yarpreader.cpp yarpwriter.cpp yarpimpl.h)

add_executable(test1 test1.cpp)

target_link_libraries(test1 yarpc)
