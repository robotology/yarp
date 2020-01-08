/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_LOGICRESOURCE
#define YARP_MANAGER_LOGICRESOURCE

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/resource.h>
#include <yarp/conf/api.h>

namespace yarp {
namespace manager {


class Platform : public GenericResource
{
public:
    Platform(void);
    Platform(const char* szName);
    virtual ~Platform();
    Node* clone(void) override;
    bool satisfy(GenericResource* resource) override;

    void setDistribution(const char* str) { if(str) strDistrib = str; }
    void setRelease(const char* str) { if(str) strRelease = str; }
    const char* getDistribution(void) { return strDistrib.c_str(); }
    const char* getRelease(void) { return strRelease.c_str(); }

protected:

private:
    std::string strDistrib;
    std::string strRelease;
    bool satisfy_platform(Platform* os);

};


class ResYarpPort : public GenericResource
{
public:
    ResYarpPort(void);
    ResYarpPort(const char* szName);
    virtual ~ResYarpPort();
    void setPort(const char* szPort) { if(szPort) strPort = szPort; }
    const char* getPort(void) { return strPort.c_str(); }
    void setRequest(const char* szReq) { if(szReq) strRequest = szReq; }
    void setReply(const char* szRep) { if(szRep) strReply = szRep; }
    const char* getRequest(void) { return strRequest.c_str(); }
    const char* getReply(void) { return strReply.c_str(); }
    void setTimeout(double t) { timeout = t; }
    double getTimeout(void) { return timeout; }
    Node* clone(void) override;
    bool satisfy(GenericResource* resource) override;

protected:

private:
    std::string strPort;
    std::string strRequest;
    std::string strReply;
    double timeout;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_LOGICRESOURCE__
