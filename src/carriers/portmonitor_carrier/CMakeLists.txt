# Copyright (C) 2014 iCub Facility
# Authors: Ali Paikan and Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(portmonitor_carrier TYPE PortMonitor INCLUDE PortMonitor.h)
  yarp_add_carrier_fingerprint(portmonitor.ini portmonitor_carrier)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_portmonitor_carrier)

  include_directories(${YARP_INCLUDE_DIRS}
                      ${CMAKE_CURRENT_SOURCE_DIR}/lua
                      ${CMAKE_CURRENT_SOURCE_DIR}/dll)
  include_directories(SYSTEM ${LUA_INCLUDE_DIR})

  yarp_add_plugin(yarp_portmonitor
                  PortMonitor.h
                  MonitorBinding.h
                  MonitorEvent.h
                  PortMonitor.cpp
                  MonitorBinding.cpp
                  lua/MonitorLua.h
                  lua/MonitorLua.cpp
                  dll/MonitorSharedLib.h
                  dll/MonitorSharedLib.cpp)

  target_link_libraries(yarp_portmonitor YARP_OS ${LUA_LIBRARY})
  target_link_libraries(yarp_portmonitor ${ACE_LIBRARIES})

endif ()

