/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP2_WIREIMAGE4MJPEG
#define YARP2_WIREIMAGE4MJPEG

#include <yarp/os/SizedWriter.h>
#include <yarp/sig/Image.h>

class WireImage4mjpeg {
private:
    yarp::sig::FlexImage img;
public:
    yarp::sig::FlexImage *checkForImage(yarp::os::SizedWriter& writer);
};

#endif
