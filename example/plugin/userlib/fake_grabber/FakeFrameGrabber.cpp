/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "FakeFrameGrabber.h"
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

bool FakeFrameGrabber::getImage(ImageOf<PixelRgb>& image) {
    Time::delay(0.5);  // simulate waiting for hardware to report
    image.resize(w,h);
    image.zero();
    return true;
}
