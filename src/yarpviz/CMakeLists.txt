# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(YARP_COMPILE_yarpviz)

  project(yarpviz)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_subdirectory(src)
  add_subdirectory(app)

endif()
