# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(portmonitor
  TYPE PortMonitor
  INCLUDE PortMonitor.h
  CATEGORY carrier
  EXTRA_CONFIG SUBTYPE=recv
    CODE="not applicable"
  DEFAULT ON
)

if (NOT SKIP_portmonitor)
  yarp_add_plugin(yarp_portmonitor)

  target_sources(yarp_portmonitor
    PRIVATE
      PortMonitor.h
      MonitorBinding.h
      MonitorEvent.h
      MonitorLogComponent.h
      PortMonitor.cpp
      MonitorBinding.cpp
      MonitorLogComponent.cpp
      dll/MonitorSharedLib.h
      dll/MonitorSharedLib.cpp
  )

  if(YARP_HAS_Lua)
    target_sources(yarp_portmonitor
      PRIVATE
        lua/MonitorLua.cpp
        lua/MonitorLua.h
    )
    if(YARP_HAS_SWIG AND NOT CMAKE_CROSSCOMPILING)
      include(${SWIG_USE_FILE})
      add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/lua/src_gen/swigluarun.h"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/lua/src_gen/"
        COMMAND "${CMAKE_COMMAND}" -E env "SWIG_LIB=${SWIG_DIR}" "${SWIG_EXECUTABLE}" -c++ -lua -external-runtime "${CMAKE_CURRENT_BINARY_DIR}/lua/src_gen/swigluarun.h"
        COMMENT "Generating swig-lua runtime"
      )
      target_include_directories(yarp_portmonitor PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/lua/src_gen")
      target_sources(yarp_portmonitor PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/lua/src_gen/swigluarun.h")
    else()
      target_include_directories(yarp_portmonitor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lua/src_gen")
    endif()
  endif()

  if(YARP_HAS_Python)
    target_sources(yarp_portmonitor
      PRIVATE
        python/MonitorPython.cpp
        python/MonitorPython.h
    )
    if(YARP_HAS_SWIG AND NOT CMAKE_CROSSCOMPILING)
      include(${SWIG_USE_FILE})
      add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/python/src_gen/swigpythonrun.h"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/python/src_gen/"
        COMMAND "${CMAKE_COMMAND}" -E env "SWIG_LIB=${SWIG_DIR}" "${SWIG_EXECUTABLE}" -c++ -python -external-runtime "${CMAKE_CURRENT_BINARY_DIR}/python/src_gen/swigpythonrun.h"
        COMMENT "Generating swig-python runtime"
      )
      target_include_directories(yarp_portmonitor PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/python/src_gen")
      target_sources(yarp_portmonitor PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/python/src_gen/swigpythonrun.h")
    else()
      target_include_directories(yarp_portmonitor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/python/src_gen")
    endif()
  endif()

  target_include_directories(yarp_portmonitor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                                      ${CMAKE_CURRENT_SOURCE_DIR}/dll)

  target_link_libraries(yarp_portmonitor PRIVATE YARP::YARP_os
                                                 YARP::YARP_dev
                                                 YARP::YARP_sig)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os)

  if(YARP_HAS_Lua)
    target_include_directories(yarp_portmonitor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lua)
    target_include_directories(yarp_portmonitor SYSTEM PRIVATE ${LUA_INCLUDE_DIR})
    target_compile_definitions(yarp_portmonitor PRIVATE ENABLED_PORTMONITOR_LUA)
    target_link_libraries(yarp_portmonitor PRIVATE ${LUA_LIBRARY})
#     list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS Lua) (not using targets)
  endif()

  if(YARP_HAS_Python)
    target_include_directories(yarp_portmonitor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/python)
    target_include_directories(yarp_portmonitor SYSTEM PRIVATE ${PYTHON_INCLUDE_DIR})
    target_compile_definitions(yarp_portmonitor PRIVATE ENABLED_PORTMONITOR_PYTHON)
    target_link_libraries(yarp_portmonitor PRIVATE Python::Python)
  endif()

  yarp_install(
    TARGETS yarp_portmonitor
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_portmonitor PROPERTY FOLDER "Plugins/Carrier")
endif()
