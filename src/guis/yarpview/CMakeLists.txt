# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpview)

  include(YarpUseQt5)

  set(QtYARPViewPlugin_QMLDIR "robotology/yarp/view")

  add_subdirectory(plugin)
  add_subdirectory(src)

endif()
