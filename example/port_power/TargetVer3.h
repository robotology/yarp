// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef TARGETVER1_INC
#define TARGETVER1_INC

#include <yarp/os/Portable.h>

class Target : public yarp::os::Portable {
public:
  int x;
  int y;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
      connection.appendInt(BOTTLE_TAG_LIST+BOTTLE_TAG_INT);
      connection.appendInt(2); // two elements
      connection.appendInt(x);
      connection.appendInt(y);
      connection.convertTextMode(); // if connection is text-mode, convert!
      return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
      connection.convertTextMode(); // if connection is text-mode, convert!
      int tag = connection.expectInt();
      x = y = -1;
      if (tag!=BOTTLE_TAG_LIST+BOTTLE_TAG_INT) return false;
      int ct = connection.expectInt();
      if (ct!=2) return false;
      x = connection.expectInt();
      y = connection.expectInt();
      return true;
  }
};

#endif
