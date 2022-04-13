/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SOUNDMARKER_H
#define SOUNDMARKER_H

#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>
#include <yarp/sig/Sound.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

class Sound_marker : public yarp::os::MonitorObject
{
    yarp::os::Things m_th;
    yarp::sig::Sound m_s2;

public:
    void getParamsFromCommandLine(std::string carrierString, yarp::os::Property& prop);

    bool create(const yarp::os::Property &options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property &params) override;
    bool getparam(yarp::os::Property &params) override;

    void trig() override;;

    bool accept(yarp::os::Things &thing) override;
    yarp::os::Things &update(yarp::os::Things &thing) override;
};

#endif
