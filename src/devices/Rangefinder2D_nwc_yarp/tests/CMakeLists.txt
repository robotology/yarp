# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(TARGET YARP::YARP_math)
  create_device_test(rangefinder2D_nwc_yarp YARP::YARP_math)
endif()
