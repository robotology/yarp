#  Yarp Modules Manager
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

project(YARP_manager)

file(GLOB folder_source src/*.cpp tinyxml/*.cpp)
file(GLOB_RECURSE folder_header include/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
  ${PROJECT_SOURCE_DIR}/include)
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})

if(CREATE_LIB_MATH)
    ADD_DEFINITIONS( -DWITH_YARPMATH )
    add_library(YARP_manager STATIC ${folder_source} ${folder_header})
    target_link_libraries(YARP_manager YARP_sig YARP_OS YARP_math ${TinyXML_LIBRARIES})
else(CREATE_LIB_MATH)
    add_library(YARP_manager STATIC ${folder_source} ${folder_header})
    target_link_libraries(YARP_manager YARP_sig YARP_OS ${TinyXML_LIBRARIES})	
endif(CREATE_LIB_MATH)


#install(TARGETS YARP_manager EXPORT YARP COMPONENT runtime DESTINATION ${CMAKE_INSTALL_LIBDIR})
#set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_manager)
#set_property(TARGET YARP_manager PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
#install(DIRECTORY include/yarp/manager COMPONENT development DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)
