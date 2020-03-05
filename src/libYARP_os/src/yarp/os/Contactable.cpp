/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Contactable.h>

#include <yarp/os/AbstractContactable.h>

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
#include <yarp/os/PortReport.h>
YARP_WARNING_POP
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#endif // YARP_NO_DEPRECATED

yarp::os::Contactable::~Contactable() = default;

std::string yarp::os::Contactable::getName() const
{
    return where().getName();
}

void yarp::os::Contactable::setReadOnly()
{
    setInputMode(true);
    setOutputMode(false);
    setRpcMode(false);
}

void yarp::os::Contactable::setWriteOnly()
{
    setInputMode(false);
    setOutputMode(true);
    setRpcMode(false);
}

void yarp::os::Contactable::setRpcServer()
{
    setInputMode(true);
    setOutputMode(false);
    setRpcMode(true);
}

void yarp::os::Contactable::setRpcClient()
{
    setInputMode(false);
    setOutputMode(true);
    setRpcMode(true);
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
void yarp::os::Contactable::getReport(PortReport& reporter)
{
    getReport([&reporter](const yarp::os::PortInfo& info) { reporter.report(info); });
}

void yarp::os::Contactable::setReporter(PortReport& reporter)
{
    setReporter([&reporter](const yarp::os::PortInfo& info) { reporter.report(info); });
}
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
