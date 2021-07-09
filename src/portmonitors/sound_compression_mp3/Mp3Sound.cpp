/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Mp3Sound.h"

#include <algorithm>
#include <cmath>

#include <yarp/os/LogComponent.h>
#include <yarp/sig/SoundFile.h>

using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(MP3TOSOUND,
                   "yarp.carrier.portmonitor.sound_compression_mp3",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}

bool Mp3SoundConverter::create(const yarp::os::Property& options)
{
    senderSide = (options.find("sender_side").asBool());
    return true;
}

void Mp3SoundConverter::destroy()
{
}

bool Mp3SoundConverter::setparam(const yarp::os::Property& params)
{
    return false;
}

bool Mp3SoundConverter::getparam(yarp::os::Property& params)
{
    return false;
}

bool Mp3SoundConverter::accept(yarp::os::Things& thing)
{
    if (senderSide)
    {
        yCError(MP3TOSOUND, "accept(): Sender side not yet supported!");
        return false;
    }
    else
    {
        auto* bot = thing.cast_as<Bottle>();
        if(bot == nullptr || bot->size()!=1)
        {
            yCError(MP3TOSOUND, "Data cannot be accepted!");
            return false;
        }
    }
    return true;
}

yarp::os::Things& Mp3SoundConverter::update(yarp::os::Things& thing)
{
    if (senderSide)
    {
        yCError(MP3TOSOUND, "update(): Sender side not yet supported!");
    }
    else
    {
        yarp::os::Bottle* bot = thing.cast_as<Bottle>();
        if (bot)
        {
            const char* binary_data = bot->get(0).asBlob();
            size_t length = bot->get(0).asBlobLength();

            yarp::sig::file::read_bytestream(snd, binary_data, length, ".mp3");
        }
        else
        {
            yCError(MP3TOSOUND, "Conversion failed!");
        }
    }

    th.setPortWriter(&snd);
    return th;
}
