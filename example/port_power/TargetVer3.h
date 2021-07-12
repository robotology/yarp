/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TARGETVER3_INC
#define TARGETVER3_INC

#include <yarp/os/Portable.h>

class Target : public yarp::os::Portable {
public:
  int x;
  int y;
  bool write(yarp::os::ConnectionWriter& connection) const override {
      connection.appendInt32(BOTTLE_TAG_LIST+BOTTLE_TAG_INT32);
      connection.appendInt32(2); // two elements
      connection.appendInt32(x);
      connection.appendInt32(y);
      connection.convertTextMode(); // if connection is text-mode, convert!
      return true;
  }
  bool read(yarp::os::ConnectionReader& connection) override {
      connection.convertTextMode(); // if connection is text-mode, convert!
      int tag = connection.expectInt32();
      x = y = -1;
      if (tag!=BOTTLE_TAG_LIST+BOTTLE_TAG_INT32) return false;
      int ct = connection.expectInt32();
      if (ct!=2) return false;
      x = connection.expectInt32();
      y = connection.expectInt32();
      return true;
  }
};

#endif
