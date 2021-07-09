/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/Property.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;


int main() {
    PolyDriver dd;
    Property fakebot;
    fakebot.put("device","fakebot");
    fakebot.put("background",""); // suppress background image
    fakebot.put("target","");     // suppress foreground image
    dd.open(fakebot);

    if (!dd.isValid()) {
        printf("fakebot not available\n");
        exit(1);
    }

    printf("*** fakebot created\n");

    PolyDriver dd2("fakeFrameGrabber");
    if (!dd2.isValid()) {
        printf("fakeFrameGrabber not available\n");
        exit(1);
    }


    IFrameGrabberImage *grabber;
    dd2.view(grabber);

    if (grabber!=NULL) {
        printf("*** It can supply images\n");
        ImageOf<PixelRgb> img;
        if (grabber->getImage(img)) {
            printf("*** Got a %dx%d image\n", img.width(), img.height());
        } else {
            printf("*** Failed to actually read an image\n");
        }
    } else {
        printf("*** It can <<<<<NOT>>>>> supply images\n");
    }

    IPidControl *pid;
    dd.view(pid);

    if (pid!=NULL) {
        printf("*** It can do PID control\n");
    } else {
        printf("*** It can <<<<<NOT>>>>> do PID control\n");
    }

    return 0;
}
