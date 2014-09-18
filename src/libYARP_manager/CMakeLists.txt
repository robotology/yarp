#  Yarp Modules Manager
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

project(YARP_manager)

set(YARP_manager_HDRS include/yarp/manager/application.h
                      include/yarp/manager/arbitrator.h
                      include/yarp/manager/binexparser.h
                      include/yarp/manager/broker.h
                      include/yarp/manager/data.h
                      include/yarp/manager/execstate.h
                      include/yarp/manager/executable.h
                      include/yarp/manager/fsm.h
                      include/yarp/manager/graph.h
                      include/yarp/manager/kbase.h
                      include/yarp/manager/localbroker.h
                      include/yarp/manager/logicresource.h
                      include/yarp/manager/manager.h
                      include/yarp/manager/manifestloader.h
                      include/yarp/manager/module.h
                      include/yarp/manager/node.h
                      include/yarp/manager/physicresource.h
                      include/yarp/manager/primresource.h
                      include/yarp/manager/resource.h
                      include/yarp/manager/scriptbroker.h
                      include/yarp/manager/singleapploader.h
                      include/yarp/manager/utility.h
                      include/yarp/manager/xmlapploader.h
                      include/yarp/manager/xmlappsaver.h
                      include/yarp/manager/xmlmodloader.h
                      include/yarp/manager/xmlresloader.h
                      include/yarp/manager/xmltemploader.h
                      include/yarp/manager/yarpbroker.h
                      include/yarp/manager/yarpdevbroker.h
                      include/yarp/manager/ymm-dir.h
                      include/yarp/manager/ymm-types.h)

set(YARP_manager_IMPL_HDRS )

set(YARP_manager_SRCS src/application.cpp
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

source_group("Source Files" FILES ${YARP_manager_SRCS})
source_group("Header Files" FILES ${YARP_manager_HDRS})
source_group("Implementation Header Files" FILES ${YARP_manager_IMPL_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})


if(CREATE_LIB_MATH)
  add_definitions( -DWITH_YARPMATH )
endif()


# FIXME All API should use a YARP_manager_API for __declspec(dllimport/dllexport)
# For now always build the library as STATIC
add_library(YARP_manager STATIC ${YARP_manager_SRCS}
                                ${YARP_manager_HDRS}
                                ${YARP_manager_IMPL_HDRS})

target_link_libraries(YARP_manager LINK_PRIVATE YARP_sig YARP_OS)
target_link_libraries(YARP_manager LINK_PRIVATE ${TinyXML_LIBRARIES})

if(CREATE_LIB_MATH)
  target_link_libraries(YARP_manager LINK_PRIVATE YARP_math)
endif()

install(TARGETS YARP_manager
        EXPORT YARP
        COMPONENT runtime
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/yarp/manager
        COMPONENT development
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp
        FILES_MATCHING PATTERN "*.h"
        PATTERN ".*" EXCLUDE)

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_manager)
set_property(TARGET YARP_manager PROPERTY INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
