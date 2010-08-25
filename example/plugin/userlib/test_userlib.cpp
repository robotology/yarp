/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
using namespace yarp::os;
using namespace yarp::dev;
YARP_DECLARE_PLUGINS(userlib);
int main(int argc, char *argv[]) {
  Network yarp;
  YARP_REGISTER_PLUGINS(userlib);
  PolyDriver dd("fake_grabber");
  if (!dd.isValid()) {
    printf("fake_grabber not available\n");
    return 1;
  }
  printf("Was able to instantiate a fake_grabber\n");
  return 0;
}
