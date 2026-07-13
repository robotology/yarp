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
        yarp::sig::Sound* snd = thing.cast_as<yarp::sig::Sound>();
        if(snd == nullptr)
        {
            yCError(MP3TOSOUND, "Sender: Data cannot be accepted!");
            return false;
        }
    }
    else
    {
        auto* bot = thing.cast_as<Bottle>();
        if(bot == nullptr)
        {
            yCError(MP3TOSOUND, "Receiver: Data cannot be accepted, invalid datatype");
            return false;
        }
        if(bot->size() != 1)
        {
            yCError(MP3TOSOUND, "Receiver: Data cannot be accepted, invalid format");
            return false;
        }
    }
    return true;
}

yarp::os::Things& Mp3SoundConverter::update(yarp::os::Things& thing)
{
    if (senderSide)
    {
        yarp::sig::Sound* snd = thing.cast_as<yarp::sig::Sound>();
        if (snd)
        {
            size_t lenght=0;
            bool success = yarp::sig::file::write_bytestream(*snd, nullptr, lenght, ".mp3");
            if (!success || lenght == 0)
            {
                yCError(MP3TOSOUND, "Failed in allocating buffer size!");
                return m_th;
            }
            char* bs = new char[lenght];
            success = yarp::sig::file::write_bytestream(*snd, bs, lenght, ".mp3");
            if (success)
            {
                m_bot.clear();
                yarp::os::Value v(bs, lenght);
                m_bot.add(v);
            }
            else
            {
                yCError(MP3TOSOUND, "Encoding failed!");
            }
            delete[] bs;

            m_th.setPortWriter(&m_bot);
        }
        else
        {
            yCError(MP3TOSOUND, "Conversion failed!");
        }
    }
    else
    {
        yarp::os::Bottle* bot = thing.cast_as<Bottle>();
        if (bot)
        {
            const char* binary_data = bot->get(0).asBlob();
            size_t length = bot->get(0).asBlobLength();

            bool success = yarp::sig::file::read_bytestream(m_snd, binary_data, length, ".mp3");
            m_th.setPortWriter(&m_snd);
        }
        else
        {
            yCError(MP3TOSOUND, "Conversion failed!");
        }
    }

    return m_th;
}
