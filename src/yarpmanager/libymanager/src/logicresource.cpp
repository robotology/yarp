/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "logicresource.h"
#include <string.h>

/**
 * Class Platform
 */
Platform::Platform(void) : GenericResource("Platform") 
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

Node* Platform::clone(void)
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
ResYarpPort::ResYarpPort(void) : GenericResource("ResYarpPort") 
{
    timeout = 0;
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
}

Node* ResYarpPort::clone(void)
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


