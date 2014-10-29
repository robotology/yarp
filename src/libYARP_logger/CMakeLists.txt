#  Yarp Logger
#  Copyright: (C) 2014 iCub Facility - Istituto Italiano di Tecnologia (IIT)
#  Authors: Marco Randazzo <marco.randazzo@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

project(YARP_logger)

set(YARP_logger_HDRS include/yarp/logger/YarpLogger.h)

set(YARP_logger_IMPL_HDRS )

set(YARP_logger_SRCS src/YarpLogger.cpp)

source_group("Source Files" FILES ${YARP_logger_SRCS})
source_group("Header Files" FILES ${YARP_logger_HDRS})
source_group("Implementation Header Files" FILES ${YARP_logger_impl_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})


if(CREATE_LIB_MATH)
  add_definitions( -DWITH_YARPMATH )
endif()


# FIXME All API should use a YARP_logger_API for __declspec(dllimport/dllexport)
# For now always build the library as STATIC
add_library(YARP_logger STATIC ${YARP_logger_SRCS}
                                ${YARP_logger_HDRS}
                                ${YARP_logger_IMPL_HDRS})

target_link_libraries(YARP_logger LINK_PRIVATE YARP_sig YARP_OS)
target_link_libraries(YARP_logger LINK_PRIVATE ${TinyXML_LIBRARIES})

if(CREATE_LIB_MATH)
  target_link_libraries(YARP_logger LINK_PRIVATE YARP_math)
endif()

set_property(TARGET YARP_logger PROPERTY PUBLIC_HEADER ${YARP_logger_HDRS})
set_property(TARGET YARP_logger PROPERTY PRIVATE_HEADER ${YARP_logger_IMPL_HDRS})

install(TARGETS YARP_logger
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/logger"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/logger/impl")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_logger)
set_property(TARGET YARP_logger PROPERTY INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
