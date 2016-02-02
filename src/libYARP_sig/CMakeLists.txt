# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_sig)

set(YARP_sig_HDRS include/yarp/sig/all.h
                  include/yarp/sig/api.h
                  include/yarp/sig/ImageDraw.h
                  include/yarp/sig/ImageFile.h
                  include/yarp/sig/Image.h
                  include/yarp/sig/ImageNetworkHeader.h
                  include/yarp/sig/Matrix.h
                  include/yarp/sig/SoundFile.h
                  include/yarp/sig/Sound.h
                  include/yarp/sig/Vector.h
                  include/yarp/sig/PointCloud.hpp
                  include/yarp/sig/PointCloud_NetworkHeader.hpp
                  include/yarp/sig/PointCloudTypes.hpp)

if(NOT YARP_NO_DEPRECATED) # Since YARP 3.0.0
  list(APPEND YARP_sig_HDRS include/yarp/sig/IplImage.h)
endif()

set(YARP_sig_IMPL_HDRS include/yarp/sig/impl/DeBayer.h
                       include/yarp/sig/impl/IplImage.h)

set(YARP_sig_SRCS src/ImageCopy.cpp
                  src/Image.cpp
                  src/ImageFile.cpp
                  src/IplImage.cpp
                  src/Matrix.cpp
                  src/Sound.cpp
                  src/SoundFile.cpp
                  src/Vector.cpp
                  src/DeBayer.cpp
                  src/PointCloud.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_sig_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_sig_HDRS}
                   ${YARP_sig_IMPL_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
# YARP_sig library uses headers from YARP_OS impl, and therefore ACE headers
# are also required.
include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

add_library(YARP_sig ${YARP_sig_SRCS}
                     ${YARP_sig_HDRS}
                     ${YARP_sig_IMPL_HDRS})
add_library(YARP::YARP_sig ALIAS YARP_sig)

target_include_directories(YARP_sig PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_sig PRIVATE ${ACE_LIBRARIES}
                               PUBLIC YARP::YARP_conf
                                      YARP::YARP_OS)

if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
  message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
endif()
if(CMAKE_VERSION VERSION_LESS 3.1)
  if(DEFINED CXX11_FLAGS)
    target_compile_options(YARP_sig PUBLIC ${CXX11_FLAGS})
  endif()
else()
  target_compile_features(YARP_sig PUBLIC cxx_override)
endif()

set_property(TARGET YARP_sig PROPERTY PUBLIC_HEADER ${YARP_sig_HDRS})
set_property(TARGET YARP_sig PROPERTY PRIVATE_HEADER ${YARP_sig_IMPL_HDRS})

install(TARGETS YARP_sig
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/sig
        PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/sig/impl)

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_sig)
set_property(TARGET YARP_sig PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
set_property(TARGET YARP_sig PROPERTY FOLDER "Libraries")
