/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Sound_marker.h"
#include <yarp/os/LogComponent.h>
#include <yarp/sig/SoundFilters.h>

using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(SOUND_MARKER, "sound_marker")

void split(const std::string& s, char delim, std::vector<std::string>& elements)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        elements.push_back(item);
    }
}
} //anonymous namespace


void Sound_marker::getParamsFromCommandLine(std::string carrierString, yarp::os::Property& prop)
{
    // Split command line string using '+' delimiter
    std::vector<std::string> parameters;
    split(carrierString, '+', parameters);

    // Iterate over result strings
    for (std::string param : parameters)
    {
        // If there is no '.', then the param is bad formatted, skip it.
        auto pointPosition = param.find('.');
        if (pointPosition == std::string::npos)
        {
            continue;
        }

        // Otherwise, separate key and value
        std::string paramKey = param.substr(0, pointPosition);
        yarp::os::Value paramValue;
        std::string s = param.substr(pointPosition + 1, param.length());
        paramValue.fromString(s.c_str());

        //and append to the returned property
        prop.put(paramKey, paramValue);
    }
    return;
}

bool Sound_marker::create(const yarp::os::Property &options)
{
    yCDebug(SOUND_MARKER, "created!\n");
    yCDebug(SOUND_MARKER, "I am attached to the %s\n",
            (options.find("sender_side").asBool()) ? "sender side" : "receiver side");

    //parse the user parameters
    yarp::os::Property m_user_params;
    yCDebug(SOUND_MARKER) << "user params:" << options.toString();
    std::string str = options.find("carrier").asString();
    getParamsFromCommandLine(str, m_user_params);
    yCDebug(SOUND_MARKER) << "parsed params:" << m_user_params.toString();

    return true;
}

void Sound_marker::destroy()
{
}

bool Sound_marker::setparam(const yarp::os::Property &params)
{
    return false;
}

bool Sound_marker::getparam(yarp::os::Property &params)
{
    return false;
}

bool Sound_marker::accept(yarp::os::Things &thing)
{
    yarp::sig::Sound *bt = thing.cast_as<yarp::sig::Sound>();
    if (bt == NULL)
    {
        yCWarning(SOUND_MARKER, "expected type Sound but got wrong data type!\n");
        return false;
    }

    return true;
}

yarp::os::Things & Sound_marker::update(yarp::os::Things &thing)
{
    //get data to process
    yarp::sig::Sound *s = thing.cast_as<yarp::sig::Sound>();
    if (s == NULL)
    {
        yCWarning(SOUND_MARKER, "expected type Sound but got wrong data type!\n");
        return thing;
    }

    size_t ss_size = s->getSamples();
    size_t ch_size = s->getChannels();
    m_s2 = *s;
    m_s2.resize(ss_size+5, ch_size);

    for (size_t c = 0; c < ch_size; c++)
    {
        for (size_t i = 0; i < ss_size; i++)
        {
            m_s2.set(s->get(i,c), i, c);
        }
        for (size_t i = ss_size; i < ss_size+5; i++)
        {
            m_s2.set(32000,i,c);
        }
        m_s2.set(-32000, 0, c);
    }

    //send data
    m_th.setPortWriter(&m_s2);
    return m_th;
}

void Sound_marker::trig()
{
}
