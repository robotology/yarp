# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(portmonitor
                    TYPE yarp::os::PortMonitor
                    INCLUDE PortMonitor.h
                    CATEGORY carrier
                    EXTRA_CONFIG SUBTYPE=recv
                                 CODE="not applicable"
                    DEFAULT ON)

if (NOT SKIP_portmonitor)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dll)


  if(LUA_FOUND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lua)
    include_directories(SYSTEM ${LUA_INCLUDE_DIR})
    set(portmonitor_LUA_SRCS lua/MonitorLua.cpp lua/MonitorLua.h)
    add_definitions("-DENABLED_PORTMONITOR_LUA")
  endif()

  yarp_add_plugin(yarp_portmonitor
                  PortMonitor.h
                  MonitorBinding.h
                  MonitorEvent.h
                  PortMonitor.cpp
                  MonitorBinding.cpp
                  dll/MonitorSharedLib.h
                  dll/MonitorSharedLib.cpp
                  ${portmonitor_LUA_SRCS})

  target_link_libraries(yarp_portmonitor YARP::YARP_OS)

  if(LUA_FOUND)
    target_link_libraries(yarp_portmonitor ${LUA_LIBRARY})
  endif()

  yarp_install(TARGETS yarp_portmonitor
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES portmonitor.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_portmonitor PROPERTY FOLDER "Plugins/Carrier")
endif()
