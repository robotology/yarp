# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpscope)

  include(YarpUseQt5)

  set(QtYARPScopePlugin_QMLDIR "robotology/yarp/scope")

  add_subdirectory(plugin)
  add_subdirectory(src)

endif()
