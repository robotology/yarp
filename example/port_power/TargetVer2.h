/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef TARGETVER2_INC
#define TARGETVER2_INC

#include <yarp/os/Portable.h>

class Target : public yarp::os::Portable {
public:
  int x;
  int y;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(x);
    connection.appendInt(y);
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    x = connection.expectInt();
    y = connection.expectInt();
    return true;
  }
};

#endif
