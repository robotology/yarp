/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
