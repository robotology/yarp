# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPSCOPE AND YARP_HAS_QT5)

  include(YarpUseQt5)
  
  set(QtYARPScopePlugin_QMLDIR "robotology/yarp/scope")

  add_subdirectory(plugin)
  add_subdirectory(src)

endif()