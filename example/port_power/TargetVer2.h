/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef TARGETVER2_INC
#define TARGETVER2_INC

#include <yarp/os/Portable.h>

class Target : public yarp::os::Portable {
public:
  int x;
  int y;
  bool write(yarp::os::ConnectionWriter& connection) const override {
    connection.appendInt32(x);
    connection.appendInt32(y);
    return true;
  }
  bool read(yarp::os::ConnectionReader& connection) override {
    x = connection.expectInt32();
    y = connection.expectInt32();
    return true;
  }
};

#endif
