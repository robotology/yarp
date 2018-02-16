# Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Do not build extern stuff with experimental warnings
if(YARP_EXPERIMENTAL_WARNINGS)
  string(REPLACE "${EXPERIMENTAL_WARNING_FLAGS} " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

# Do not build with -Wno-suggest-override
string(REPLACE "-Wsuggest-override" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

# TinyXML
if(YARP_BUILD_TINYXML)
  add_subdirectory(tinyxml)
  set(TinyXML_INCLUDE_DIRS ${TinyXML_INCLUDE_DIRS} PARENT_SCOPE)
  set(TinyXML_LIBRARIES ${TinyXML_LIBRARIES} PARENT_SCOPE)
  set(TinyXML_DEFINITIONS ${TinyXML_DEFINITIONS} PARENT_SCOPE)
endif()

# SQLite
if(YARP_BUILD_SQLITE)
  add_subdirectory(sqlite)
  set(SQLite_INCLUDE_DIRS ${SQLite_INCLUDE_DIRS} PARENT_SCOPE)
  set(SQLite_LIBRARIES ${SQLite_LIBRARIES} PARENT_SCOPE)
  set(SQLite_DEFINITIONS ${SQLite_DEFINITIONS} PARENT_SCOPE)
endif()

# QCustomPlot
if(YARP_BUILD_QCUSTOMPLOT)
  add_subdirectory(qcustomplot)
  set(QCustomPlot_INCLUDE_DIRS ${QCustomPlot_INCLUDE_DIRS} PARENT_SCOPE)
  set(QCustomPlot_LIBRARIES ${QCustomPlot_LIBRARIES} PARENT_SCOPE)
  set(QCustomPlot_DEFINITIONS ${QCustomPlot_DEFINITIONS} PARENT_SCOPE)
endif()

# XmlRpc++
if(YARP_BUILD_XMLRPCPP)
  add_subdirectory(xmlrpcpp)
  set(xmlrpcpp_INCLUDE_DIRS ${xmlrpcpp_INCLUDE_DIRS} PARENT_SCOPE)
  set(xmlrpcpp_LIBRARIES ${xmlrpcpp_LIBRARIES} PARENT_SCOPE)
  set(xmlrpcpp_DEFINITIONS ${xmlrpcpp_DEFINITIONS} PARENT_SCOPE)
endif()

# Qgv
if(YARP_BUILD_QGVCORE)
  add_subdirectory(qgv)
  set(QGVCore_INCLUDE_DIRS ${QGVCore_INCLUDE_DIRS} PARENT_SCOPE)
  set(QGVCore_LIBRARIES ${QGVCore_LIBRARIES} PARENT_SCOPE)
  set(QGVCore_DEFINITIONS ${QGVCore_DEFINITIONS} PARENT_SCOPE)
endif()

# hmac
if(YARP_BUILD_HMAC)
  add_subdirectory(hmac)
  set(hmac_INCLUDE_DIRS ${hmac_INCLUDE_DIRS} PARENT_SCOPE)
  set(hmac_LIBRARIES ${hmac_LIBRARIES} PARENT_SCOPE)
  set(hmac_DEFINITIONS ${hmac_DEFINITIONS} PARENT_SCOPE)
endif()

# libdc1394_bayer
set(libdc1394_bayer_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libdc1394_bayer" PARENT_SCOPE)

# rplidar SDK
set(RPLIDAR_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/rplidar/sdk" PARENT_SCOPE)

# diff_match_patch
set(diff_match_patch_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/diff_match_patch" PARENT_SCOPE)
