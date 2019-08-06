# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(YARP_COMPILE_yarprobotinterface)

  include_directories(${TinyXML_INCLUDE_DIRS})

  set(yarprobotinterface_SRCS
      main.cpp
      Action.cpp
      CalibratorThread.cpp
      Device.cpp
      Module.cpp
      Param.cpp
      Robot.cpp
      Types.cpp
      XMLReaderVx.cpp
      XMLReaderV1.cpp
      XMLReaderV3.cpp
      RobotInterfaceDTD.cpp)

  set(yarprobotinterface_HDRS
      Action.h
      CalibratorThread.h
      Device.h
      Module.h
      Param.h
      Robot.h
      Types.h
      XMLReader.h
      RobotInterfaceDTD.h)

  set(yarprobotinterface_IDLS
      yarprobotinterface.thrift)

  include(YarpIDL)
  yarp_idl_to_dir(INPUT_FILES ${yarprobotinterface_IDLS}
                  OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code"
                  SOURCES_VAR yarprobotinterface_IDLS_SRCS
                  HEADERS_VAR yarprobotinterface_IDLS_HDRS
                  INCLUDE_DIRS_VAR yarprobotinterface_IDLS_INCLUDE_DIRS)
  include_directories(${yarprobotinterface_IDLS_INCLUDE_DIRS})

  add_executable(yarprobotinterface ${yarprobotinterface_SRCS}
                                    ${yarprobotinterface_HDRS}
                                    ${yarprobotinterface_IDLS_SRCS}
                                    ${yarprobotinterface_IDLS_HDRS})

  target_link_libraries(yarprobotinterface
                        YARP::YARP_os
                        YARP::YARP_dev
                        YARP::YARP_init
                        ${TinyXML_LIBRARIES})

  install(TARGETS yarprobotinterface
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarprobotinterface PROPERTY FOLDER "Command Line Tools")
endif()
