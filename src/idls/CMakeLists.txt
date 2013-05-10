# Copyright: (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(CREATE_IDLS "Compile (very experimental) IDL compilers" FALSE)

if (CREATE_IDLS)
   option(ENABLE_yarpidl_thrift "Use a thrift-based IDL" OFF)
   if (ENABLE_yarpidl_thrift)
     add_subdirectory(thrift)
   endif ()
   option(ENABLE_yarpidl_rosmsg "Use a ROS Msg based IDL" OFF)
   if (ENABLE_yarpidl_rosmsg OR ENABLE_yarpcar_tcpros_carrier)
     add_subdirectory(rosmsg)
   endif ()
endif ()
