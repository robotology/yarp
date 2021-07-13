# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(tcpros
  CATEGORY carrier
  TYPE TcpRosCarrier
  INCLUDE TcpRosCarrier.h
  EXTRA_CONFIG
    CODE=\(any\)\ \(any\)\ \"\\\\0\\\\0\"\ \(any\)\ \(any\)\ \"\\\\0\\\\0\"
  DEFAULT ON
)

yarp_prepare_plugin(rossrv
  CATEGORY carrier
  TYPE RosSrvCarrier
  INCLUDE TcpRosCarrier.h
  DEFAULT ON
)

if(NOT SKIP_tcpros OR NOT SKIP_rossrv)
  yarp_add_plugin(yarp_tcpros)

  target_sources(yarp_tcpros
    PRIVATE
      TcpRosCarrier.h
      TcpRosCarrier.cpp
      TcpRosLogComponent.h
      TcpRosLogComponent.cpp
      TcpRosStream.h
      TcpRosStream.cpp
      RosHeader.h
      RosHeader.cpp
      RosSlave.h
      RosLookup.h
      RosLookup.cpp
  )

  target_link_libraries(yarp_tcpros
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_wire_rep_utils
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_wire_rep_utils
  )

  yarp_install(
    TARGETS yarp_tcpros
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_tcpros PROPERTY FOLDER "Plugins/Carrier")

  # yarpros executable
  if(YARP_COMPILE_EXECUTABLES)
    add_executable(yarpros)

    target_sources(yarpros
      PRIVATE
        yarpros.cpp
        TcpRosLogComponent.h
        TcpRosLogComponent.cpp
        TcpRosStream.cpp
        RosLookup.cpp
    )

    target_link_libraries(yarpros
      PRIVATE
        YARP::YARP_os
        YARP::YARP_init
        YARP::YARP_wire_rep_utils
    )

    install(
      TARGETS yarpros
      EXPORT YARP_${YARP_PLUGIN_MASTER}
      COMPONENT utilities
      DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    set_property(TARGET yarpros PROPERTY FOLDER "Command Line Tools")
  endif()
endif()

include(YarpRemoveFile)
yarp_remove_file("${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}/ros.ini") # YARP_DEPRECATED since YARP 3.2
