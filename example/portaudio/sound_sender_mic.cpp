/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/os/Time.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

const int rec_seconds = 1;

int main(int argc, char *argv[]) {

    // Open the network
    Network yarp;
    Port p;
    p.open("/sender");

    // Get a portaudio read device.
    Property conf;
    conf.put("device","portaudio");
    conf.put("read", "");
    conf.put("samples", 44100*rec_seconds);
    //conf.put("rate", 16000);
    PolyDriver poly(conf);
    IAudioGrabberSound *get;

    // Make sure we can read sound
    poly.view(get);
    if (get==NULL) {
        printf("cannot open interface\n");
        return 1;
    }

    //Grab and send
    Sound s;

    get->startRecording(); //this is optional, the first get->getsound() will do this anyway.
    while (true)
    {
        double t1=yarp::os::Time::now();
        get->getSound(s);
        double t2=yarp::os::Time::now();
        printf("acquired %f seconds\n", t2-t1);
        p.write(s);
    }
    get->stopRecording();  //stops recording.

    return 0;
}

