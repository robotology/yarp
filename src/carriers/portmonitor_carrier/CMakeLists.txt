# Copyright (C) 2014 iCub Facility
# Authors: Ali Paikan and Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(portmonitor
                    TYPE yarp::os::PortMonitor
                    INCLUDE PortMonitor.h
                    CATEGORY carrier
                    EXTRA_CONFIG SUBTYPE=recv
                                 CODE="not applicable"
                    DEPENDS "CREATE_OPTIONAL_CARRIERS;YARP_HAS_ACE"
                    DEFAULT ON)

if (NOT SKIP_portmonitor)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dll)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS})

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

  target_link_libraries(yarp_portmonitor YARP_OS)
  if(LUA_FOUND)
    target_link_libraries(yarp_portmonitor ${LUA_LIBRARY})
  endif()
  target_link_libraries(yarp_portmonitor ${ACE_LIBRARIES})
  yarp_install(TARGETS yarp_portmonitor
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES portmonitor.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
