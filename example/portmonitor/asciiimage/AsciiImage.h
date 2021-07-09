/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
