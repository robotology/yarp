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

add_subdirectory(carriers)

if (CREATE_OPTIONAL_CARRIERS)
  add_library(YARP_OS_base ${folder_source} ${folder_header})
  set_property(SOURCE init/CustomInit.cpp APPEND PROPERTY COMPILE_DEFINITIONS 
               PLUGIN_INIT_FUNCTION add_yarpcar_devices)
  add_library(YARP_OS init/CustomInit.cpp)
  target_link_libraries(YARP_OS yarpcar YARP_OS_base)
  install(TARGETS YARP_OS YARP_OS_base EXPORT YARP COMPONENT runtime DESTINATION lib)
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_OS YARP_OS_base)
else (CREATE_OPTIONAL_CARRIERS)
  add_library(YARP_OS ${folder_source} ${folder_header} init/CustomInit.cpp)
  install(TARGETS YARP_OS EXPORT YARP COMPONENT runtime DESTINATION lib)
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_OS)
endif (CREATE_OPTIONAL_CARRIERS)

add_executable(yarp yarp/yarp.cpp)
target_link_libraries(yarp YARP_OS ${ACE_LINK_FLAGS})

add_executable(yarpserver yarp/yarpserver.cpp)
target_link_libraries(yarpserver YARP_OS ${ACE_LINK_FLAGS})

set_property(TARGET YARP_OS PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

install(TARGETS yarp yarpserver COMPONENT utilities DESTINATION bin)

install(DIRECTORY include/yarp/os COMPONENT development DESTINATION include/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)


