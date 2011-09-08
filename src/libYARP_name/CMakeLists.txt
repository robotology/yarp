# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (YARP_USE_PERSISTENT_NAMESERVER)
 
  project(YARP_name)

  file(GLOB folder_source src/*.cpp)
  file(GLOB_RECURSE folder_header include/*.h)
  source_group("Source Files" FILES ${folder_source})
  source_group("Header Files" FILES ${folder_header})

  set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
    ${PROJECT_SOURCE_DIR}/include)
  get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(${ACE_INCLUDE_DIRS})

  add_library(YARP_name ${folder_source} ${folder_header})
  target_link_libraries(YARP_name YARP_OS)
  install(TARGETS YARP_name EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_name)

  set_property(TARGET YARP_name PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

  install(DIRECTORY include/yarp/name COMPONENT development DESTINATION include/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)

endif()
