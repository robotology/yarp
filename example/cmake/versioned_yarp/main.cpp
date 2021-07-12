/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <yarp/os/all.h>

int main() {
  printf("Hello from YARP [dramatic pause]\n");
  yarp::os::Time::delay(5);
  printf("[/dramatic pause]\n");
  return 0;
}
