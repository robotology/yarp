# Copyright: (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


project(YARP_rtf)


set(YARP_rtf_HDRS include/yarp/rtf/JointsPosMotion.h
                  include/yarp/rtf/TestAsserter.h
                  include/yarp/rtf/TestCase.h)

set(YARP_rtf_SRCS src/JointsPosMotion.cpp
                  src/TestAsserter.cpp
                  src/TestCase.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_rtf_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_rtf_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(SYSTEM ${RTF_INCLUDE_DIRS})

add_library(YARP_rtf ${YARP_rtf_SRCS} ${YARP_rtf_HDRS})
add_library(YARP::YARP_rtf ALIAS YARP_rtf)

target_link_libraries(YARP_rtf PUBLIC RTF::RTF
                               PRIVATE YARP::YARP_OS
                                       YARP::YARP_sig
                                       YARP::YARP_init
                                       YARP::YARP_dev)

set_property(TARGET YARP_rtf PROPERTY PUBLIC_HEADER ${YARP_rtf_HDRS})

install(TARGETS YARP_rtf
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/rtf)

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_rtf)
set_property(TARGET YARP_rtf PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
set_property(TARGET YARP_rtf PROPERTY FOLDER "Libraries")
