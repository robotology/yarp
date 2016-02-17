# Copyright (C) 2014 iCub Facility
# Authors: Ali Paikan and Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(portmonitor_carrier TYPE yarp::os::PortMonitor INCLUDE PortMonitor.h)
  yarp_install(FILES portmonitor.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif (COMPILE_PLUGIN_LIBRARY)


if (NOT SKIP_portmonitor_carrier)

  if(LUA_FOUND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lua)
    include_directories(SYSTEM ${LUA_INCLUDE_DIR})
    set(portmonitor_LUA_SRCS lua/MonitorLua.cpp lua/MonitorLua.h)
    add_definitions(" -DENABLED_PORTMONITOR_LUA")
  endif ()

  include_directories(${YARP_INCLUDE_DIRS}                      
                      ${CMAKE_CURRENT_SOURCE_DIR}/dll)  
  yarp_add_plugin(yarp_portmonitor
                  PortMonitor.h
                  MonitorBinding.h
                  MonitorEvent.h
                  PortMonitor.cpp
                  MonitorBinding.cpp
                  dll/MonitorSharedLib.h
                  dll/MonitorSharedLib.cpp
                  ${portmonitor_LUA_SRCS})

  target_link_libraries(yarp_portmonitor YARP_OS ${LUA_LIBRARY})
  target_link_libraries(yarp_portmonitor ${ACE_LIBRARIES})
  yarp_install(TARGETS yarp_portmonitor
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

endif ()

