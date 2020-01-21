/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <cmath>

#include <yarp/os/all.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
    Network yarp;

    // Get an audio device.

    Property p;
    if (argc>1) {
        p.fromCommand(argc,argv);
    } else {
        p.fromString("(device portaudio)");
    }
    PolyDriver poly(p);
    if (!poly.isValid()) {
        printf("cannot open driver\n");
        return 1;
    }


    // Make sure we can both read and write sound

    IAudioGrabberSound *get;
    IAudioRender *put;
    poly.view(get);
    poly.view(put);
    if (get==NULL&&put==NULL) {
        printf("cannot open interface\n");
        return 1;
    }


    // echo from microphone to headphones, superimposing an annoying tone

    double vv = 0;
    while (true) {
        Sound s;
        get->getSound(s);
        for (int i=0; i<s.getSamples(); i++) {
            double now = Time::now();
            static double first = now;
            now -= first;
            if ((long int) (now*2) % 2 == 0) {
                vv += 0.08;
            } else {
                vv += 0.04;
            }
            double dv = 500*sin(vv);
            for (int j=0; j<s.getChannels(); j++) {
                int v = s.get(i,j);
                s.set((int)(v+dv+0.5),i,j);
            }
        }
        put->renderSound(s);
    }

    return 0;
}
