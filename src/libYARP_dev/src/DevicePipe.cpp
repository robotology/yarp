// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    bool ok = open("source",source,config,
                   "device to read from (string or nested properties)");
    if (!ok) return false;
    ok = open("sink",sink,config,"device to write to (string or nested properties)");
    if (!ok) {
        source.close();
        return false;
    }
    return true;
}


bool DevicePipe::open(const char *key, PolyDriver& poly,
                      yarp::os::Searchable& config, const char *comment) {

    Value *name;
    if (config.check(key,name,comment)) {
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
            Bottle subdevice = config.findGroup(key).tail();
            poly.open(subdevice);
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
    IAudioGrabberSound *sndSource;
    IAudioVisualGrabber *imgSndSource;
    IAudioVisualStream *sourceType;

    IAudioRender *sndSink;
    IFrameWriterImage *imgSink;
    IFrameWriterAudioVisual *imgSndSink;
    IAudioVisualStream *sinkType;

    source.view(imgSource);
    source.view(sndSource);
    source.view(imgSndSource);
    source.view(sourceType);

    sink.view(imgSink);
    sink.view(sndSink);
    sink.view(imgSndSink);
    sink.view(sinkType);

    if (sourceType!=NULL) {
        if (!(sourceType->hasAudio()&&sourceType->hasVideo())) {
            imgSndSource = NULL;
        }
    }
    if (sinkType!=NULL) {
        if (!(sinkType->hasAudio()&&sinkType->hasVideo())) {
            imgSndSink = NULL;
        }
    }


    if (imgSndSource!=NULL&&imgSndSink!=NULL) {
        ImageOf<PixelRgb> tmp;
        Sound tmpSound;
        imgSndSource->getAudioVisual(tmp,tmpSound);
        imgSndSink->putAudioVisual(tmp,tmpSound);
        printf("piped %dx%d image, %dx%d sound\n",
               tmp.width(), tmp.height(),
               tmpSound.getSamples(), tmpSound.getChannels());
    } else if (imgSource!=NULL&&imgSink!=NULL) {
        ImageOf<PixelRgb> tmp;
        imgSource->getImage(tmp);
        imgSink->putImage(tmp);
        printf("piped %dx%d image\n", tmp.width(), tmp.height());
    } else if (sndSource!=NULL&&sndSink!=NULL) {
        Sound tmp;
        sndSource->getSound(tmp);
        sndSink->renderSound(tmp);
        printf("piped %dx%d sound\n", tmp.getSamples(), tmp.getChannels());
    } else {
        printf("Don't know how to pipe between these devices.\n");
        printf("Piping is very limited at the moment.\n");
        printf("You're probably better off writing some short custom code.\n");
        return false;
    }
    return true;
}
