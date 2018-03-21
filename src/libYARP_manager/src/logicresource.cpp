/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

Platform::Platform(const Platform &resource) : GenericResource(resource)
{
    strDistrib = resource.strDistrib;
    strRelease = resource.strRelease;
}

Node* Platform::clone()
{
    Platform* resource = new Platform(*this);
    return resource;
}

bool Platform::satisfy(GenericResource* resource)
{
    if(!getAvailability() || getDisable())
        return false;

    Platform* os = dynamic_cast<Platform*>(resource);
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

Platform::~Platform() { }



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

ResYarpPort::ResYarpPort(const ResYarpPort &resource) : GenericResource(resource)
{
    timeout = resource.timeout;
    strPort = resource.strPort;
    strRequest = resource.strRequest;
    strReply = resource.strReply;
}

Node* ResYarpPort::clone()
{
    ResYarpPort* resource = new ResYarpPort(*this);
    return resource;
}

bool ResYarpPort::satisfy(GenericResource* resource)
{
    if(!getAvailability() || getDisable())
        return false;

    ResYarpPort* resport = dynamic_cast<ResYarpPort*>(resource);
    if(!resport)
        return false;
    return (strPort == string(resport->getPort()) ||
            strPort == string(resport->getName()) );
}

ResYarpPort::~ResYarpPort() { }
