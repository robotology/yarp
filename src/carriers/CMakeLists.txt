# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(CREATE_OPTIONAL_CARRIERS "Compile some optional carriers" FALSE)

get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
include_directories(${YARP_OS_INCLUDE_DIRS})

add_subdirectory(wire_rep_utils)

include(YarpPlugin)
yarp_begin_plugin_library(yarpcar)
  add_subdirectory(human_carrier)
  add_subdirectory(mpi_carrier)
  add_subdirectory(xmlrpc_carrier)
  add_subdirectory(tcpros_carrier)
  add_subdirectory(mjpeg_carrier)
  add_subdirectory(bayer_carrier)
  add_subdirectory(priority_carrier)
  add_subdirectory(portmonitor_carrier)
  add_subdirectory(depth_image_portmonitor)
  yarp_end_plugin_library(yarpcar)
install(TARGETS yarpcar
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

if(YARP_LINK_PLUGINS)
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS yarpcar ${yarpcar_LIBRARIES})
endif()

include(YarpDeprecatedOption)
yarp_renamed_option(ENABLE_yarpcar_bayer_carrier ENABLE_yarpcar_bayer) # since YARP 2.3.68
yarp_renamed_option(ENABLE_yarpcar_human_carrier ENABLE_yarpcar_human) # since YARP 2.3.68
yarp_renamed_option(ENABLE_yarpcar_mjpeg_carrier ENABLE_yarpcar_mjpeg) # since YARP 2.3.68
yarp_renamed_option(ENABLE_yarpcar_mpi_carrier ENABLE_yarpcar_mpi) # since YARP 2.3.68
yarp_renamed_option(ENABLE_yarpcar_mpibcast_carrier ENABLE_yarpcar_mpibcast) # since YARP 2.3.68
yarp_renamed_option(ENABLE_yarpcar_portmonitor_carrier ENABLE_yarpcar_portmonitor) # since YARP 2.3.68
yarp_renamed_option(ENABLE_yarpcar_priority_carrier ENABLE_yarpcar_priority) # since YARP 2.3.68
yarp_renamed_option(ENABLE_yarpcar_tcpros_carrier ENABLE_yarpcar_tcpros) # since YARP 2.3.68
yarp_renamed_option(ENABLE_yarpcar_rossrv_carrier ENABLE_yarpcar_rossrv) # since YARP 2.3.68
yarp_renamed_option(ENABLE_yarpcar_xmlrpc_carrier ENABLE_yarpcar_xmlrpc) # since YARP 2.3.68
