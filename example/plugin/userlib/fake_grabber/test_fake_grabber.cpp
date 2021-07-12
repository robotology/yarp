/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "FakeFrameGrabber.h"
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
int main(int argc, char *argv[]) {
  DriverCreator *fake_factory =
    new DriverCreatorOf<FakeFrameGrabber>("fake_grabber","grabber","FakeFrameGrabber");
  Drivers::factory().add(fake_factory); // hand factory over to YARP

  PolyDriver dd("fake_grabber");
  if (!dd.isValid()) {
    printf("fake_grabber not available\n");
    return 1;
  }
  IFrameGrabberImage *grabber;
  dd.view(grabber);
  if (grabber==NULL) {
    printf("*** Device failed to supply images\n");
    return 1;
  }
  printf("*** Device can supply images\n");
  ImageOf<PixelRgb> img;
  if (grabber->getImage(img)) {
    printf("*** Got a %dx%d image\n", img.width(), img.height());
  } else {
    printf("Failed to read an image\n");
    return 1;
  }
  return 0;
}
