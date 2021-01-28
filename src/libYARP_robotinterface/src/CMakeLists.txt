# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.


add_library(YARP_robotinterface)
add_library(YARP::YARP_robotinterface ALIAS YARP_robotinterface)

set(YARP_robotinterface_HDRS yarp/robotinterface/api.h)

set(YARP_robotinterface_EXP_HDRS yarp/robotinterface/experimental/Action.h
                                 yarp/robotinterface/experimental/Device.h
                                 yarp/robotinterface/experimental/Param.h
                                 yarp/robotinterface/experimental/Robot.h
                                 yarp/robotinterface/experimental/Types.h
                                 yarp/robotinterface/experimental/XMLReader.h)

set(YARP_robotinterface_IMPL_HDRS yarp/robotinterface/impl/CalibratorThread.h
                                  yarp/robotinterface/impl/RobotInterfaceDTD.h
                                  yarp/robotinterface/impl/XMLReaderFileVx.h
                                  yarp/robotinterface/impl/XMLReaderFileV1.h
                                  yarp/robotinterface/impl/XMLReaderFileV3.h)

set(YARP_robotinterface_SRCS )

set(YARP_robotinterface_EXP_SRCS yarp/robotinterface/experimental/Action.cpp
                                 yarp/robotinterface/experimental/Device.cpp
                                 yarp/robotinterface/experimental/Param.cpp
                                 yarp/robotinterface/experimental/Robot.cpp
                                 yarp/robotinterface/experimental/Types.cpp
                                 yarp/robotinterface/experimental/XMLReader.cpp)

set(YARP_robotinterface_IMPL_SRCS yarp/robotinterface/impl/CalibratorThread.cpp
                                  yarp/robotinterface/impl/RobotInterfaceDTD.cpp
                                  yarp/robotinterface/impl/XMLReaderFileV1.cpp
                                  yarp/robotinterface/impl/XMLReaderFileV3.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_robotinterface_SRCS}
                   ${YARP_robotinterface_EXP_SRCS}
                   ${YARP_robotinterface_IMPL_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_robotinterface_HDRS}
                   ${YARP_robotinterface_EXP_HDRS}
                   ${YARP_robotinterface_IMPL_HDRS})

target_sources(YARP_robotinterface PRIVATE ${YARP_robotinterface_SRCS}
                                           ${YARP_robotinterface_HDRS}
                                           ${YARP_robotinterface_EXP_SRCS}
                                           ${YARP_robotinterface_EXP_HDRS}
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
set_property(TARGET YARP_robotinterface PROPERTY EXPERIMENTAL_HEADER ${YARP_robotinterface_EXP_HDRS})
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

install(FILES ${YARP_robotinterface_EXP_HDRS}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/robotinterface/experimental"
        COMPONENT YARP_robotinterface-dev)

set(YARP_robotinterface_PUBLIC_DEPS ${YARP_robotinterface_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_robotinterface_PRIVATE_DEPS ${YARP_robotinterface_PRIVATE_DEPS} PARENT_SCOPE)
