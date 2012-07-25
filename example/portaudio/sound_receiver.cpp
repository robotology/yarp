/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Francesco Nori
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[]) {

    // Open the network
    Network yarp;
    BufferedPort<Sound> p;
    p.open("/receiver");
    Network::connect("/sender", "/receiver");

    // Get an audio write device.
    Property conf;
    conf.put("device","portaudio");
    conf.put("samples", "4096");
    conf.put("write", "1");
    PolyDriver poly(conf);
    IAudioRender *put;

    // Make sure we can write sound
    poly.view(put);
    if (put==NULL) {
        printf("cannot open interface\n");
        return 1;
    }

    //Receive and render
    Sound *s;
    while (true)
      {
	s = p.read(false);
	if (s!=NULL)
	    put->renderSound(*s);
      }
    return 0;
}

