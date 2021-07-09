/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
