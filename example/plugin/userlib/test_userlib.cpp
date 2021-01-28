/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
