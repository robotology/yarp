# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpviz)

  project(yarpviz)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_subdirectory(src)
  add_subdirectory(app)

endif()
