// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/dev/DeviceDriver.h>

#include <ace/OS.h>

using namespace yarp::dev;
using namespace yarp::os;

// the commented out commands are deprecated

/*
  bool DeviceDriver::apply(yarp::os::ConnectionReader& cmd) {
  yarp::os::Bottle in, out;
  in.read(cmd);
  bool result = apply(in,out);
  if (result) {
  if (out.size()>0) {
  yarp::os::ConnectionWriter *writer = cmd.getWriter();
  if (writer!=NULL) {
  out.write(*writer);
  }
  }
  }
  return result;
  }

  bool DeviceDriver::apply(yarp::os::Bottle& cmd, 
  yarp::os::Bottle& response) {
  switch (cmd.getInt(0)) {
  case 1:
  ACE_OS::printf("hello, a dummy command was called\n");
  response.clear();
  response.addInt(42);
  return true;
  break;
  }
  return false;
  }

*/
