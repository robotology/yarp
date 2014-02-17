#  Yarp Modules Manager
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

project(YARP_manager)

set(folder_source   src/application.cpp
                    src/arbitrator.cpp
                    src/binexparser.cpp
                    src/broker.cpp
                    src/data.cpp
                    src/execstate.cpp
                    src/executable.cpp
                    src/graph.cpp
                    src/kbase.cpp
                    src/localbroker.cpp
                    src/logicresource.cpp
                    src/manager.cpp
                    src/module.cpp
                    src/node.cpp
                    src/physicresource.cpp
                    src/primresource.cpp
                    src/resource.cpp
                    src/scriptbroker.cpp
                    src/singleapploader.cpp
                    src/utility.cpp
                    src/xmlapploader.cpp
                    src/xmlappsaver.cpp
                    src/xmlmodloader.cpp
                    src/xmlresloader.cpp
                    src/xmltemploader.cpp
                    src/yarpbroker.cpp)

set(folder_header   include/application.h
                    include/arbitrator.h
                    include/binexparser.h
                    include/broker.h
                    include/data.h
                    include/execstate.h
                    include/executable.h
                    include/fsm.h
                    include/graph.h
                    include/kbase.h
                    include/localbroker.h
                    include/logicresource.h
                    include/manager.h
                    include/manifestloader.h
                    include/module.h
                    include/node.h
                    include/physicresource.h
                    include/primresource.h
                    include/resource.h
                    include/scriptbroker.h
                    include/singleapploader.h
                    include/utility.h
                    include/xmlapploader.h
                    include/xmlappsaver.h
                    include/xmlmodloader.h
                    include/xmlresloader.h
                    include/xmltemploader.h
                    include/yarpbroker.h
                    include/yarpdevbroker.h
                    include/ymm-dir.h
                    include/ymm-types.h)

source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
  ${PROJECT_SOURCE_DIR}/include)
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})


if(CREATE_LIB_MATH)
    add_definitions( -DWITH_YARPMATH )
endif()

add_library(YARP_manager STATIC ${folder_source} ${folder_header})
target_link_libraries(YARP_manager LINK_PRIVATE YARP_sig YARP_OS)
target_link_libraries(YARP_manager LINK_PRIVATE ${TinyXML_LIBRARIES})

if(CREATE_LIB_MATH)
    target_link_libraries(YARP_manager LINK_PRIVATE YARP_math)
endif()


#install(TARGETS YARP_manager EXPORT YARP COMPONENT runtime DESTINATION ${CMAKE_INSTALL_LIBDIR})
#set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_manager)
#set_property(TARGET YARP_manager PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
#install(DIRECTORY include/yarp/manager COMPONENT development DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)
