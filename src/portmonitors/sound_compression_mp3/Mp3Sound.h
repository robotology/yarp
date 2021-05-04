/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MP3SOUND_CONVERTER_H
#define YARP_MP3SOUND_CONVERTER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/sig/Sound.h>
#include <yarp/os/MonitorObject.h>

//example usage:
//yarp connect /src /dest tcp+recv.portmonitor+type.dll+file.sound_compression_mp3

class Mp3SoundConverter : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;

private:
    bool senderSide;
    yarp::sig::Sound snd;
    yarp::os::Things th;
};

#endif  // YARP_MP3SOUND_CONVERTER_H
