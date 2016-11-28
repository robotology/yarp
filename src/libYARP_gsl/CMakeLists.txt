# Copyright: (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_gsl)

set(YARP_gsl_HDRS include/yarp/gsl/api.h
                  include/yarp/gsl/Gsl.h)
set(YARP_gsl_SRCS src/Gsl.cpp)
set(YARP_gsl_IMPL_HDRS include/yarp/gsl_compatibility.h)

source_group("Source Files" FILES ${YARP_gsl_SRCS})
source_group("Header Files" FILES ${YARP_gsl_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})

add_library(YARP_gsl ${YARP_gsl_SRCS} ${YARP_gsl_HDRS})
target_link_libraries(YARP_gsl LINK_PRIVATE YARP_sig)

set_property(TARGET YARP_gsl PROPERTY PUBLIC_HEADER ${YARP_gsl_HDRS})
set_property(TARGET YARP_gsl PROPERTY PRIVATE_HEADER ${YARP_gsl_IMPL_HDRS})

install(TARGETS YARP_gsl
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/gsl
        PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/gsl/impl)

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_gsl)
set_property(TARGET YARP_gsl PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
