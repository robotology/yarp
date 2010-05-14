# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 
project(libYARP_dev)

file(GLOB folder_source src/*.cpp)
file(GLOB_RECURSE folder_header include/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

#set(populator ${CMAKE_BINARY_DIR}/src/libYARP_dev/src_generated/PopulateDrivers.cpp)

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
  ${PROJECT_SOURCE_DIR}/include)
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})

#configure_file(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/PopulateDrivers.cpp.in
#  ${populator} @ONLY  IMMEDIATE)


add_library(YARP_dev ${folder_source} ${folder_header} ${populator})
target_link_libraries(YARP_dev YARP_sig YARP_OS)
install(TARGETS YARP_dev EXPORT YARP COMPONENT runtime DESTINATION lib)
set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_dev)

set_property(TARGET YARP_dev PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

install(DIRECTORY include/yarp/dev COMPONENT development DESTINATION include/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)
