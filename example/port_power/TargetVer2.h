// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef TARGETVER1_INC
#define TARGETVER1_INC

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
