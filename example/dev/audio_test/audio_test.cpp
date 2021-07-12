/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>

#include <yarp/sig/Sound.h>

#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>

#include <cmath>
#include <cstdio>

using yarp::os::Network;
using yarp::os::Property;
using yarp::sig::Sound;
using yarp::dev::IAudioGrabberSound;
using yarp::dev::IAudioRender;
using yarp::dev::PolyDriver;

int main(int argc, char* argv[])
{
    Network yarp;

    // Get an audio device.

    Property p;
    if (argc > 1) {
        p.fromCommand(argc, argv);
    } else {
        p.fromString("(device portaudio)");
    }
    PolyDriver poly(p);
    if (!poly.isValid()) {
        printf("cannot open driver\n");
        return 1;
    }


    // Make sure we can both read and write sound

    IAudioGrabberSound* get;
    IAudioRender* put;
    poly.view(get);
    poly.view(put);
    if (get == nullptr && put == nullptr) {
        printf("cannot open interface\n");
        return 1;
    }


    // echo from microphone to headphones, superimposing an annoying tone

    double vv = 0;
    while (true) {
        Sound s;
        get->getSound(s, 100, 100000, 0.0);
        for (size_t i = 0; i < s.getSamples(); i++) {
            double now = yarp::os::Time::now();
            static double first = now;
            now -= first;
            if (static_cast<long int>(now * 2) % 2 == 0) {
                vv += 0.08;
            } else {
                vv += 0.04;
            }
            double dv = 500 * sin(vv);
            for (size_t j = 0; j < s.getChannels(); j++) {
                int v = s.get(i, j);
                s.set(static_cast<int>(v + dv + 0.5), i, j);
            }
        }
        put->renderSound(s);
    }

    return 0;
}
