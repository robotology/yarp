/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef SIMPLE_INC
#define SIMPLE_INC
#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>
#include <yarp/sig/Sound.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

class MicrophoneFilter : public yarp::os::MonitorObject
{
    yarp::os::Things m_th;
    yarp::sig::Sound m_s2;
    int m_channel = 0;
    int m_output_freq = 16000;

public:
    bool create(const yarp::os::Property &options);
    void destroy();

    bool setparam(const yarp::os::Property &params);
    bool getparam(yarp::os::Property &params);

    void trig();

    bool accept(yarp::os::Things &thing);
    yarp::os::Things &update(yarp::os::Things &thing);
};

#endif
