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

int main() {
  printf("Hello from YARP [dramatic pause]\n");
  yarp::os::Time::delay(5);
  printf("[/dramatic pause]\n");
  return 0;
}
