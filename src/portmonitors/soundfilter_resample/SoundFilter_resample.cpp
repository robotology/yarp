/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SoundFilter_resample.h"
#include <yarp/os/LogComponent.h>
#include <yarp/sig/SoundFilters.h>

using namespace yarp::os;

YARP_LOG_COMPONENT(SOUNDFILTER_RESAMPLE, "soundfilter_resample")

bool SoundFilter_resample::create(const yarp::os::Property &options)
{
    yCDebug(SOUNDFILTER_RESAMPLE, "created!\n");
    yCDebug(SOUNDFILTER_RESAMPLE, "I am attached to the %s\n",
            (options.find("sender_side").asBool()) ? "sender side" : "receiver side");

    m_channel = (options.check("channel") ? options.find("channel").asInt8() : -1);
    m_output_freq = (options.check("frequency") ? options.find("frequency").asInt32() : -1);

    if (m_channel < 0)
    {
        yCInfo(SOUNDFILTER_RESAMPLE, "no channels selected for extraction, using all sound channels");
    }
    else
    {
        yCInfo(SOUNDFILTER_RESAMPLE, "channel selected: %d", m_channel);
    }

    if (m_output_freq < 0)
    {
        yCInfo(SOUNDFILTER_RESAMPLE, "No resampling requested");
    }
    else if (m_output_freq == 0)
    {
        yCError(SOUNDFILTER_RESAMPLE, "Invalid frequency value selected");
        return false;
    }
    else
    {
        yCInfo(SOUNDFILTER_RESAMPLE, "output frequency : %d", m_output_freq);
    }

    return true;
}

void SoundFilter_resample::destroy()
{
}

bool SoundFilter_resample::setparam(const yarp::os::Property &params)
{
    m_channel = params.find("channel").asInt8();
    m_output_freq = params.find("frequency").asInt32();
    return false;
}

bool SoundFilter_resample::getparam(yarp::os::Property &params)
{
    params.put("channel", m_channel);
    params.put("frequency", m_output_freq);
    return false;
}

bool SoundFilter_resample::accept(yarp::os::Things &thing)
{
    yarp::sig::Sound *bt = thing.cast_as<yarp::sig::Sound>();
    if (bt == NULL)
    {
        yCWarning(SOUNDFILTER_RESAMPLE, "expected type Sound but got wrong data type!\n");
        return false;
    }

    return true;
}

yarp::os::Things & SoundFilter_resample::update(yarp::os::Things &thing)
{
    //get data to process
    yarp::sig::Sound *s = thing.cast_as<yarp::sig::Sound>();
    if (s == NULL)
    {
        yCWarning(SOUNDFILTER_RESAMPLE, "expected type Sound but got wrong data type!\n");
        return thing;
    }

    //extract one channel
    m_s2.clear();
    m_s2 = s->extractChannelAsSound(m_channel);
    if (m_output_freq>0)
    {
        yarp::sig::soundfilters::resample (m_s2,m_output_freq);
    }

    //send data
    m_th.setPortWriter(&m_s2);
    return m_th;
}

void SoundFilter_resample::trig()
{
}
