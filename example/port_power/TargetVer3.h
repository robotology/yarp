/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef TARGETVER3_INC
#define TARGETVER3_INC

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
