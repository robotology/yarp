/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SoundFilter_resample.h"
#include <yarp/os/LogComponent.h>
#include <yarp/sig/SoundFilters.h>

using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(SOUNDFILTER_RESAMPLE, "soundfilter_resample")

void split(const std::string& s, char delim, vector<string>& elements)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        elements.push_back(item);
    }
}

void SoundFilter_resample::getParamsFromCommandLine(std::string carrierString, yarp::os::Property& prop)
{
    // Split command line string using '+' delimiter
    vector<string> parameters;
    split(carrierString, '+', parameters);

    // Iterate over result strings
    for (string param : parameters)
    {
        // If there is no '.', then the param is bad formatted, skip it.
        auto pointPosition = param.find('.');
        if (pointPosition == string::npos)
        {
            continue;
        }

        // Otherwise, separate key and value
        string paramKey = param.substr(0, pointPosition);
        yarp::os::Value paramValue;
        string s = param.substr(pointPosition + 1, param.length());
        paramValue.fromString(s.c_str());

        //and append to the returned property
        prop.put(paramKey, paramValue);
    }
    return;
}

bool SoundFilter_resample::create(const yarp::os::Property &options)
{
    yCDebug(SOUNDFILTER_RESAMPLE, "created!\n");
    yCDebug(SOUNDFILTER_RESAMPLE, "I am attached to the %s\n",
            (options.find("sender_side").asBool()) ? "sender side" : "receiver side");

    //parse the user parameters
    yarp::os::Property m_user_params;
    yCDebug(SOUNDFILTER_RESAMPLE) << "user params:" << options.toString();
    std::string str = options.find("carrier").asString();
    getParamsFromCommandLine(str, m_user_params);
    yCDebug(SOUNDFILTER_RESAMPLE) << "parsed params:" << m_user_params.toString();

    //set the user parameters
    m_channel = (m_user_params.check("channel") ? m_user_params.find("channel").asInt8() : -1);
    m_output_freq = (m_user_params.check("frequency") ? m_user_params.find("frequency").asInt32() : -1);
    m_gain = (m_user_params.check("gain_percent") ? m_user_params.find("gain_percent").asFloat32() : -1)/100;

    if (m_channel < 0)
    {
        yCInfo(SOUNDFILTER_RESAMPLE, "no channels selected for extraction, using all sound channels");
    }
    else
    {
        yCInfo(SOUNDFILTER_RESAMPLE, "channel selected: %d", m_channel);
    }

    if (m_gain < 0)
    {
        yCInfo(SOUNDFILTER_RESAMPLE, "No amplification requested");
    }
    else
    {
        yCInfo(SOUNDFILTER_RESAMPLE, "Amplification gain: %f (%d%%)",  m_gain, int(m_gain*100));
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
    if (m_channel >=0)
    {
        m_s2 = s->extractChannelAsSound(m_channel);
    }
    else
    {
        m_s2 = *s;
    }

    if (m_gain >= 0)
    {
        m_s2.amplify(m_gain);
    }

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
