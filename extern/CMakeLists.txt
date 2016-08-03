# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Do not build extern stuff with experimental warnings
if(YARP_EXPERIMENTAL_WARNINGS)
  string(REPLACE "${EXPERIMENTAL_WARNING_FLAGS} " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

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
