# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
project(libYARP_dev)

file(GLOB folder_source src/*.cpp)
file(GLOB_RECURSE folder_header include/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
  ${PROJECT_SOURCE_DIR}/include)
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${ACE_INCLUDE_DIRS})

add_library(YARP_dev ${folder_source} ${folder_header})
target_link_libraries(YARP_dev LINK_PRIVATE YARP_OS)
target_link_libraries(YARP_dev LINK_PRIVATE YARP_sig)
target_link_libraries(YARP_dev LINK_PRIVATE ${ACE_LIBRARIES})
install(TARGETS YARP_dev EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})
set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_dev)

set_property(TARGET YARP_dev PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

install(DIRECTORY include/yarp/dev COMPONENT development DESTINATION include/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)
install(DIRECTORY include/yarp/dev COMPONENT development DESTINATION include/yarp FILES_MATCHING PATTERN "*.inl" PATTERN ".*" EXCLUDE)
