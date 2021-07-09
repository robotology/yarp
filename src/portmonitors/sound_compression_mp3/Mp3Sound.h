/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MP3SOUND_CONVERTER_H
#define YARP_MP3SOUND_CONVERTER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/sig/Sound.h>
#include <yarp/os/MonitorObject.h>

/**
 * \brief `sound_compression_mp3`: A portmonitor which is able to convert apply mp3 compression/decompression
 * to yarp::sig::Sound data transmitted over the network.
 *
 * Example usage:
 * yarp connect /src /dest tcp+recv.portmonitor+type.dll+file.sound_compression_mp3
 *
 * See \ref AudioDoc for additional documentation on YARP audio.
 */
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
