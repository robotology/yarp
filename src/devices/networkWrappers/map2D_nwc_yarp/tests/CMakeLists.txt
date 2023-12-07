# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(TARGET YARP::YARP_math)
  create_device_test(map2D_nwc YARP::YARP_math)
endif()
