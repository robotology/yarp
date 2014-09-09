# Copyright (C) 2012 IITRBCS
# Authors: Ali Paikan and Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(portmonitor_carrier TYPE PortMonitor INCLUDE PortMonitor.h)
  yarp_add_carrier_fingerprint(portmonitor.ini portmonitor_carrier)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_portmonitor_carrier)

  include_directories(${YARP_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} ./bindings)

  yarp_add_plugin(yarp_portmonitor
                  PortMonitor.h
                  MonitorBinding.h
                  MonitorEvent.h
                  PortMonitor.cpp
                  MonitorBinding.cpp
                  bindings/MonitorLua.h
                  bindings/MonitorLua.cpp
                  bindings/MonitorSharedLib.h
                  bindings/MonitorSharedLib.cpp)

  target_link_libraries(yarp_portmonitor YARP_OS ${LUA_LIBRARY})
  target_link_libraries(yarp_portmonitor ${ACE_LIBRARIES})

endif ()

