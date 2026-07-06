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

} //anonymous namespace


bool Sound_marker::create(const yarp::os::Property &options)
{
    yCDebug(SOUND_MARKER, "created!\n");
    yCDebug(SOUND_MARKER, "I am attached to the %s\n",
            (options.find("sender_side").asBool()) ? "sender side" : "receiver side");

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

    // add markers at the edges of the sound
    m_s2 = *s;
    m_s2.remove_all_markers();
    m_s2.add_marker("SND_MARKER_START" + std::to_string(marker_counter), 0);
    m_s2.add_marker("SND_MARKER_END"   + std::to_string(marker_counter), s->getSamples()-1);

    yDebug() << m_s2.getMarkersCount();

    //send data
    m_th.setPortWriter(&m_s2);

    marker_counter++;
    return m_th;
}

void Sound_marker::trig()
{
}
