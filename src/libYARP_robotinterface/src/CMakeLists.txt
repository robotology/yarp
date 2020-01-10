# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.


add_library(YARP_robotinterface)
add_library(YARP::YARP_robotinterface ALIAS YARP_robotinterface)

set(YARP_robotinterface_HDRS yarp/robotinterface/Action.h
                             yarp/robotinterface/api.h
                             yarp/robotinterface/CalibratorThread.h
                             yarp/robotinterface/Device.h
                             yarp/robotinterface/Param.h
                             yarp/robotinterface/Robot.h
                             yarp/robotinterface/Types.h
                             yarp/robotinterface/XMLReader.h)

set(YARP_robotinterface_IMPL_HDRS yarp/robotinterface/impl/RobotInterfaceDTD.h)

set(YARP_robotinterface_SRCS yarp/robotinterface/Action.cpp
                             yarp/robotinterface/CalibratorThread.cpp
                             yarp/robotinterface/Device.cpp
                             yarp/robotinterface/Param.cpp
                             yarp/robotinterface/Robot.cpp
                             yarp/robotinterface/Types.cpp
                             yarp/robotinterface/XMLReaderVx.cpp
                             yarp/robotinterface/XMLReaderV1.cpp
                             yarp/robotinterface/XMLReaderV3.cpp
                             yarp/robotinterface/RobotInterfaceDTD.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_robotinterface_SRCS}
                   ${YARP_robotinterface_IMPL_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_robotinterface_HDRS}
                   ${YARP_robotinterface_IMPL_HDRS})

target_sources(YARP_robotinterface PRIVATE ${YARP_robotinterface_SRCS}
                                           ${YARP_robotinterface_HDRS}
                                           ${YARP_robotinterface_IMPL_SRCS}
                                           ${YARP_robotinterface_IMPL_HDRS})

target_include_directories(YARP_robotinterface PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                                      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_robotinterface PUBLIC cxx_std_14)

target_link_libraries(YARP_robotinterface PUBLIC YARP::YARP_os
                                                 YARP::YARP_dev
                                                 YARP::YARP_init)
list(APPEND YARP_robotinterface_PUBLIC_DEPS YARP_os
                                            YARP_dev)

target_include_directories(YARP_robotinterface SYSTEM PRIVATE ${TinyXML_INCLUDE_DIRS})
target_link_libraries(YARP_robotinterface PRIVATE ${TinyXML_LIBRARIES})
list(APPEND YARP_robotinterface_PRIVATE_DEPS TinyXML)

set_property(TARGET YARP_robotinterface PROPERTY PUBLIC_HEADER ${YARP_robotinterface_HDRS})
set_property(TARGET YARP_robotinterface PROPERTY PRIVATE_HEADER ${YARP_robotinterface_IMPL_HDRS})
set_property(TARGET YARP_robotinterface PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_robotinterface PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_robotinterface PROPERTY FOLDER "Libraries")


install(TARGETS YARP_robotinterface
        EXPORT YARP_robotinterface
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_robotinterface
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_robotinterface
          NAMELINK_COMPONENT YARP_robotinterface-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_robotinterface-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/robotinterface"
          COMPONENT YARP_robotinterface-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/robotinterface/impl"
          COMPONENT YARP_robotinterface-priv-dev)


set(YARP_robotinterface_PUBLIC_DEPS ${YARP_robotinterface_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_robotinterface_PRIVATE_DEPS ${YARP_robotinterface_PRIVATE_DEPS} PARENT_SCOPE)
