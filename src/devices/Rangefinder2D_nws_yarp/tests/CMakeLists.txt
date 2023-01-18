# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(TARGET YARP::YARP_math)
  create_device_test(rangefinder2D_nws_yarp YARP::YARP_math)
endif()
