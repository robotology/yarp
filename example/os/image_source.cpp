/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;

int main(int argc, char *argv[]) {
    Network yarp;
    BufferedPort<ImageOf<PixelRgb> > port;

    Property options;
    options.fromCommand(argc,argv);
    port.open(options.check("name",Value("/image")).asString());

    int ct = 0;
    while (true) {
        ImageOf<PixelRgb>& img = port.prepare();
        img.resize(100,100);
        img.zero();
        PixelRgb blue(0,0,255);
        addCircle(img,blue,ct,50,10);
        ct = (ct+5)%100;
        port.write();
        Time::delay(0.25);
    }

    return 0;
}
