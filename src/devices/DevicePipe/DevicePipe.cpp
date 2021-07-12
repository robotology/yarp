/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DevicePipe.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/Time.h>

#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameWriterImage.h>
#include <yarp/dev/AudioVisualInterfaces.h>

#include <cstdio>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(DEVICEPIPE, "yarp.devices.DevicePipe")
}

bool DevicePipe::open(yarp::os::Searchable& config)
{
    bool ok = open("source",
                   source,
                   config,
                   "device to read from (string or nested properties)");
    if (!ok) {
        return false;
    }
    ok = open("sink",
              sink,
              config,
              "device to write to (string or nested properties)");
    if (!ok) {
        source.close();
        return false;
    }
    return true;
}


bool DevicePipe::open(const char* key,
                      PolyDriver& poly,
                      yarp::os::Searchable& config,
                      const char* comment)
{

    Value* name;
    if (config.check(key, name, comment)) {
        if (name->isString()) {
            // maybe user isn't doing nested configuration
            yarp::os::Property p;
            p.setMonitor(config.getMonitor(),
                         name->toString().c_str()); // pass on any monitoring
            p.fromString(config.toString());
            p.put("device", name->toString());
            p.unput("subdevice");
            p.unput("wrapped");
            poly.open(p);
        } else {
            Bottle subdevice = config.findGroup(key).tail();
            poly.open(subdevice);
        }
        if (!poly.isValid()) {
            yCInfo(DEVICEPIPE, "cannot make <%s>", name->toString().c_str());
            return false;
        }
    } else {
        yCInfo(DEVICEPIPE, "\"--%s <name>\" not set", key);
        return false;
    }
    return true;
}


bool DevicePipe::close()
{
    yCInfo(DEVICEPIPE, "Devices closing");
    source.close();
    sink.close();
    return true;
}


bool DevicePipe::updateService()
{
    IFrameGrabberImage* imgSource;
    IAudioGrabberSound* sndSource;
    IAudioVisualGrabber* imgSndSource;
    IAudioVisualStream* sourceType;

    IAudioRender* sndSink;
    IFrameWriterImage* imgSink;
    IFrameWriterAudioVisual* imgSndSink;
    IAudioVisualStream* sinkType;

    source.view(imgSource);
    source.view(sndSource);
    source.view(imgSndSource);
    source.view(sourceType);

    sink.view(imgSink);
    sink.view(sndSink);
    sink.view(imgSndSink);
    sink.view(sinkType);

    if (sourceType != nullptr) {
        if (!(sourceType->hasAudio() && sourceType->hasVideo())) {
            imgSndSource = nullptr;
        }
    }
    if (sinkType != nullptr) {
        if (!(sinkType->hasAudio() && sinkType->hasVideo())) {
            imgSndSink = nullptr;
        }
    }


    if (imgSndSource != nullptr && imgSndSink != nullptr) {
        ImageOf<PixelRgb> tmp;
        Sound tmpSound;
        imgSndSource->getAudioVisual(tmp, tmpSound);
        imgSndSink->putAudioVisual(tmp, tmpSound);
        yCInfo(DEVICEPIPE,
               "piped %zux%zu image, %zux%zu sound",
               tmp.width(),
               tmp.height(),
               tmpSound.getSamples(),
               tmpSound.getChannels());
    } else if (imgSource != nullptr && imgSink != nullptr) {
        ImageOf<PixelRgb> tmp;
        imgSource->getImage(tmp);
        imgSink->putImage(tmp);
        yCInfo(DEVICEPIPE, "piped %zux%zu image", tmp.width(), tmp.height());
    } else if (sndSource != nullptr && sndSink != nullptr) {
        Sound tmp;
        //the following values have been arbitrarily chosen and may be optimized.
        //4410 samples correspond to 0.1s with a frequency of 44100hz.
        sndSource->getSound(tmp, 4410, 4410, 0);
        sndSink->renderSound(tmp);
        yCInfo(DEVICEPIPE,
               "piped %zux%zu sound",
               tmp.getSamples(),
               tmp.getChannels());
    } else {
        yCInfo(DEVICEPIPE, "Don't know how to pipe between these devices.");
        yCInfo(DEVICEPIPE, "Piping is very limited at the moment.");
        yCInfo(DEVICEPIPE, "You're probably better off writing some short custom code.");
        return false;
    }
    return true;
}
