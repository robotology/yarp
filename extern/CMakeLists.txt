# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# disable all warnings for current folder and subfolders
# (unless explicitly added by the user)
get_property(_USER_CMAKE_C_FLAGS CACHE CMAKE_C_FLAGS PROPERTY VALUE)
get_property(_USER_CMAKE_CXX_FLAGS CACHE CMAKE_CXX_FLAGS PROPERTY VALUE)
set(CMAKE_C_FLAGS "-w ${_USER_CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-w ${_USER_CMAKE_CXX_FLAGS}")

# Catch2
if(YARP_COMPILE_TESTS)
  add_subdirectory(catch2)
endif()

# TinyXML
if(YARP_BUILD_TinyXML)
  add_subdirectory(tinyxml)
  set(TinyXML_INCLUDE_DIRS ${TinyXML_INCLUDE_DIRS} PARENT_SCOPE)
  set(TinyXML_LIBRARIES ${TinyXML_LIBRARIES} PARENT_SCOPE)
  set(TinyXML_DEFINITIONS ${TinyXML_DEFINITIONS} PARENT_SCOPE)
endif()

# SQLite
if(YARP_BUILD_SQLite3)
  add_subdirectory(sqlite)
  set(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIRS} PARENT_SCOPE)
  set(SQLite3_LIBRARIES ${SQLite3_LIBRARIES} PARENT_SCOPE)
  set(SQLite3_DEFINITIONS ${SQLite3_DEFINITIONS} PARENT_SCOPE)
endif()

# QCustomPlot
if(YARP_BUILD_QCustomPlot)
  add_subdirectory(qcustomplot)
  set(QCustomPlot_INCLUDE_DIRS ${QCustomPlot_INCLUDE_DIRS} PARENT_SCOPE)
  set(QCustomPlot_LIBRARIES ${QCustomPlot_LIBRARIES} PARENT_SCOPE)
  set(QCustomPlot_DEFINITIONS ${QCustomPlot_DEFINITIONS} PARENT_SCOPE)
endif()

# Qgv
if(YARP_BUILD_QGVCore)
  add_subdirectory(qgv)
  set(QGVCore_INCLUDE_DIRS ${QGVCore_INCLUDE_DIRS} PARENT_SCOPE)
  set(QGVCore_LIBRARIES ${QGVCore_LIBRARIES} PARENT_SCOPE)
  set(QGVCore_DEFINITIONS ${QGVCore_DEFINITIONS} PARENT_SCOPE)
endif()

# hmac
if(YARP_BUILD_hmac)
  add_subdirectory(hmac)
  set(hmac_INCLUDE_DIRS ${hmac_INCLUDE_DIRS} PARENT_SCOPE)
  set(hmac_LIBRARIES ${hmac_LIBRARIES} PARENT_SCOPE)
  set(hmac_DEFINITIONS ${hmac_DEFINITIONS} PARENT_SCOPE)
endif()

# Apache Thrift
set(THRIFT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thrift/thrift" PARENT_SCOPE)

# libdc1394_bayer
set(libdc1394_bayer_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libdc1394_bayer" PARENT_SCOPE)

# diff_match_patch
set(diff_match_patch_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/diff_match_patch" PARENT_SCOPE)

# dirent (for MSVC)
if(MSVC)
  set(dirent_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/dirent/dirent/include" PARENT_SCOPE)
endif()

# WebSocket
set(WebSocket_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/websocket/WebSocket" PARENT_SCOPE)
