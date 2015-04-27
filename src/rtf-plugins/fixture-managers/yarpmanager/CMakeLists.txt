#  RTF
#  Copyright: (C) 2015 Ali Paikan
#  Authors: Ali Paikan <ali.paikan@gmail.com>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#


cmake_minimum_required(VERSION 2.8.9)
set(PROJECTNAME yarpmanager)
project(${PROJECTNAME})

find_package(YARP REQUIRED)

get_property(RTF_LIBS GLOBAL PROPERTY RTF_LIBS)
get_property(RTF_TREE_INCLUDE_DIRS GLOBAL PROPERTY RTF_TREE_INCLUDE_DIRS)


include_directories(${CMAKE_SOURCE_DIR}
                    ${RTF_TREE_INCLUDE_DIRS}
                    ${YARP_INCLUDE_DIRS})

message(STATUS "YARP_LIBRARIES: " ${YARP_LIBRARIES})

message(STATUS "RTF_LIBRARIES: " ${RTF_LIBS})

link_libraries(${RTF_LIBS}
               ${YARP_LIBRARIES}
               YARP::YARP_manager)

add_library(${PROJECTNAME} MODULE YarpFixManager.h
                                  YarpFixManager.cpp)
install(TARGETS ${PROJECTNAME}
        EXPORT ${PROJECTNAME}
        COMPONENT runtime
        LIBRARY DESTINATION lib)




