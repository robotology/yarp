/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef ASCIIIMAGE_H
#define ASCIIIMAGE_H

#include <yarp/os/Bottle.h>
#include <yarp/os/MonitorObject.h>
#include <yarp/os/Things.h>


class AsciiImageMonitorObject : public yarp::os::MonitorObject
{
public:
    AsciiImageMonitorObject() = default;
    AsciiImageMonitorObject(const AsciiImageMonitorObject&) = delete;
    AsciiImageMonitorObject(AsciiImageMonitorObject&&) = delete;
    AsciiImageMonitorObject& operator=(const AsciiImageMonitorObject&) = delete;
    AsciiImageMonitorObject& operator=(AsciiImageMonitorObject&&) = delete;
    ~AsciiImageMonitorObject() override = default;

    bool create(const yarp::os::Property& options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;

private:
    yarp::os::Bottle bt;
    yarp::os::Things th;
};

#endif
