# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_OS)

file(GLOB folder_source src/*.cpp src/*.c)
file(GLOB_RECURSE folder_header include/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

option(USE_PORT_AUTHENTICATION "Use a 3-way passphrase authentication for connections to ports" FALSE)
if (USE_PORT_AUTHENTICATION)
    add_definitions(-DPORT_AUTH)
endif (USE_PORT_AUTHENTICATION)

option(ENABLE_YARPRUN_LOG "Enable yarprun log file in temp dir" OFF)
if (ENABLE_YARPRUN_LOG)
    add_definitions(-DYARPRUN_LOG)
endif(ENABLE_YARPRUN_LOG)

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}/include
)
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})

if (NOT SKIP_ACE)
  include_directories(${ACE_INCLUDE_DIRS})
endif ()

add_library(YARP_OS ${folder_source} ${folder_header})
target_link_libraries(YARP_OS LINK_PRIVATE ${ACE_LIBRARIES})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(YARP_OS pthread)
endif()

install(TARGETS YARP_OS EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})
set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_OS)
set_property(TARGET YARP_OS PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

install(DIRECTORY include/yarp/os COMPONENT development DESTINATION include/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)

# touch 1 2 3 4 5 6 7 8 9 10 11 12
