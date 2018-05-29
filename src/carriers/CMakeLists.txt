# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include(YarpPlugin)
include(YarpPrintFeature)

yarp_begin_plugin_library(yarpcar OPTION CREATE_OPTIONAL_CARRIERS
                                  DEFAULT OFF
                                  DOC "carrier plugins")
  add_subdirectory(shmem_carrier)
  add_subdirectory(human_carrier)
  add_subdirectory(mpi_carrier)
  add_subdirectory(xmlrpc_carrier)
  add_subdirectory(tcpros_carrier)
  add_subdirectory(mjpeg_carrier)
  add_subdirectory(bayer_carrier)
  add_subdirectory(priority_carrier)
  add_subdirectory(portmonitor_carrier)
  add_subdirectory(depth_image_portmonitor)
  add_subdirectory(zfp_portmonitor)
  add_subdirectory(h264_carrier)
yarp_end_plugin_library(yarpcar QUIET)
add_library(YARP::yarpcar ALIAS yarpcar)

install(TARGETS yarpcar
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

if(YARP_LINK_PLUGINS)
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS yarpcar ${yarpcar_LIBRARIES})
endif()

set_property(TARGET yarpcar PROPERTY FOLDER "Plugins/Carrier")

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
