/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/logicresource.h>

#include <cstring>


using namespace yarp::manager;
using namespace std;


/**
 * Class Platform
 */
Platform::Platform() : GenericResource("Platform")
{
}

Platform::Platform(const char* szName) : GenericResource("Platform")
{
    setName(szName);
}

Node* Platform::clone()
{
    auto* resource = new Platform(*this);
    return resource;
}

bool Platform::satisfy(GenericResource* resource)
{
    if(!getAvailability() || getDisable())
        return false;

    auto* os = dynamic_cast<Platform*>(resource);
    if(os)
        return satisfy_platform(os);

    return false;
}

bool Platform::satisfy_platform(Platform* os)
{
    bool ret = (!strlen(os->getName()))? true : (compareString(getName(), os->getName()));
    ret &= (!strlen(os->getDistribution()))? true : (compareString(strDistrib.c_str(), os->getDistribution()));
    ret &= (!strlen(os->getRelease()))? true : (compareString(strRelease.c_str(), os->getRelease()));
    return ret;
}

Platform::~Platform() = default;



/**
 * Class ResYarpPort
 */
ResYarpPort::ResYarpPort() : GenericResource("ResYarpPort")
{
    timeout = 0;
    strRequest = strReply = "";
}

ResYarpPort::ResYarpPort(const char* szName) : GenericResource("ResYarpPort")
{
    timeout = 0;
    setName(szName);
}

Node* ResYarpPort::clone()
{
    auto* resource = new ResYarpPort(*this);
    return resource;
}

bool ResYarpPort::satisfy(GenericResource* resource)
{
    if(!getAvailability() || getDisable())
        return false;

    auto* resport = dynamic_cast<ResYarpPort*>(resource);
    if(!resport)
        return false;
    return (strPort == string(resport->getPort()) ||
            strPort == string(resport->getName()) );
}

ResYarpPort::~ResYarpPort() = default;
