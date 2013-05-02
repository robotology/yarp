#  Yarp Modules Manager
#  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
# 
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

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
include_directories(${ACE_INCLUDE_DIRS})

if(CREATE_LIB_MATH)
    ADD_DEFINITIONS( -DWITH_YARPMATH )
    include_directories(${GSL_INCLUDE_DIRS})	
set(MATH_LIB_EXTRAS ${GSL_LIBRARY})
    add_library(YARP_manager STATIC ${folder_source} ${folder_header})
    target_link_libraries(YARP_manager YARP_sig YARP_OS ${TinyXML_LIBRARIES} ${MATH_LIB_EXTRAS})
else(CREATE_LIB_MATH)
    add_library(YARP_manager STATIC ${folder_source} ${folder_header})
    target_link_libraries(YARP_manager YARP_sig YARP_OS ${TinyXML_LIBRARIES})	
endif(CREATE_LIB_MATH)


#install(TARGETS YARP_manager EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})
#set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_manager)
#set_property(TARGET YARP_manager PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
#install(DIRECTORY include/yarp/manager COMPONENT development DESTINATION include/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)

