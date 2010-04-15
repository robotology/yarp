# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

project(YARP_OS)

file(GLOB folder_source src/*.cpp)
file(GLOB_RECURSE folder_header include/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
  ${PROJECT_SOURCE_DIR}/include)
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})

add_library(YARP_OS ${folder_source} ${folder_header})
install(TARGETS YARP_OS EXPORT YARP COMPONENT runtime DESTINATION lib)
set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_OS)
set_property(TARGET YARP_OS PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

install(DIRECTORY include/yarp/os COMPONENT development DESTINATION include/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)
