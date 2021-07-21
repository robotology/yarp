/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "MicrophoneFilter.h"
#include <yarp/sig/SoundFilters.h>

using namespace yarp::os;
YARP_LOG_COMPONENT(MICROPHONE_FILTER, "Microphone_Filter", yarp::os::Log::TraceType)

bool MicrophoneFilter::create(const yarp::os::Property &options)
{
    yCDebug(MICROPHONE_FILTER, "created!\n");
    yCDebug(MICROPHONE_FILTER, "I am attached to the %s\n",
            (options.find("sender_side").asBool()) ? "sender side" : "receiver side");

    m_channel = (options.check("channel") ? options.find("channel").asInt8() : 0);
    yCDebug(MICROPHONE_FILTER, "channel number : %d", m_channel);
    m_channel = (options.check("frequency") ? options.find("frequency").asInt32() : 0);
    yCDebug(MICROPHONE_FILTER, "output frequency : %d", m_output_freq);

    return true;
}

void MicrophoneFilter::destroy()
{
}

bool MicrophoneFilter::setparam(const yarp::os::Property &params)
{
    m_channel = params.find("channel").asInt8();
    m_output_freq = params.find("frequency").asInt32();
    return false;
}

bool MicrophoneFilter::getparam(yarp::os::Property &params)
{
    params.put("channel", m_channel);
    params.put("frequency", m_output_freq);
    return false;
}

bool MicrophoneFilter::accept(yarp::os::Things &thing)
{
    yarp::sig::Sound *bt = thing.cast_as<yarp::sig::Sound>();
    if (bt == NULL)
    {
        yCWarning(MICROPHONE_FILTER, "expected type Sound but got wrong data type!\n");
        return false;
    }

    return true;
}

yarp::os::Things &MicrophoneFilter::update(yarp::os::Things &thing)
{
    //get data to process
    yarp::sig::Sound *s = thing.cast_as<yarp::sig::Sound>();
    if (s == NULL)
    {
        yCWarning(MICROPHONE_FILTER, "expected type Sound but got wrong data type!\n");
        return thing;
    }

    //extract one channel
    m_s2.clear();
    m_s2 = s->extractChannelAsSound(m_channel);
    yarp::sig::soundfilters::resample (m_s2,m_output_freq);

    //send data
    m_th.setPortWriter(&m_s2);
    return m_th;
}

void MicrophoneFilter::trig()
{
}
