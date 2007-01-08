// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/dev/DevicePipe.h>
#include <yarp/os/Time.h>
#include <stdio.h>

#include <yarp/dev/AudioVisualInterfaces.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

bool DevicePipe::open(yarp::os::Searchable& config) {
    bool ok = open("source",source,config);
    if (!ok) return false;
    ok = open("sink",sink,config);
    if (!ok) {
        source.close();
        return false;
    }
    return true;
}


bool DevicePipe::open(const char *key, PolyDriver& poly, 
                      yarp::os::Searchable& config) {
    
    Value *name;
    if (config.check(key,name,"source device to wrap")) {
        if (name->isString()) {
            // maybe user isn't doing nested configuration
            yarp::os::Property p;
            p.setMonitor(config.getMonitor(),
                         name->toString().c_str()); // pass on any monitoring
            p.fromString(config.toString());
            p.put("device",name->toString());
            p.unput("subdevice");
            p.unput("wrapped");
            poly.open(p);
        } else {
            poly.open(*name);
        }
        if (!poly.isValid()) {
            printf("cannot make <%s>\n", name->toString().c_str());
            return false;
        }
    } else {
        printf("\"--%s <name>\" not set\n", key);
        return false;
    }
    return true;
}


bool DevicePipe::close() {
    printf("Devices closing\n");
    source.close();
    sink.close();
    return true;
}


bool DevicePipe::updateService() {
    IFrameGrabberImage *imgSource;
    IFrameWriterImage *imgSink;
    source.view(imgSource);
    sink.view(imgSink);
    if (imgSource!=NULL&&imgSink!=NULL) {
        ImageOf<PixelRgb> tmp;
        imgSource->getImage(tmp);
        imgSink->putImage(tmp);
        printf("piped %dx%d image\n", tmp.width(), tmp.height());
    } else {
        printf("Don't know how to pipe between these devices.\n");
        printf("Piping is very limited at the moment.\n");
        printf("You're probably better off writing some short custom code.\n");
        Time::delay(1);
    }
}


