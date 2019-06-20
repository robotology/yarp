/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/os/Property.h>
 
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>


using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[])
{
    // Open the network
    Network yarp;

    // Get an audio device.
    Property conf;
    conf.put("device", "portaudio");
    conf.put("allow-deprecated-devices", "1");
    PolyDriver poly(conf);
    IAudioGrabberSound* get;
    IAudioRender* put;

    // Make sure we can both read and write sound
    poly.view(get);
    poly.view(put);
    if (get == nullptr && put == nullptr) {
        printf("cannot open interface\n");
        return 1;
    }

    //Grab and render
    Sound s;
    while (true) {
        get->getSound(s, 100, 100000, 0.0);
        put->renderSound(s);
    }
    return 0;
}
