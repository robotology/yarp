/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    Platform();
    Platform(const char* szName);
    ~Platform() override;
    Node* clone() override;
    bool satisfy(GenericResource* resource) override;

    void setDistribution(const char* str) { if(str) { strDistrib = str; } }
    void setRelease(const char* str) { if(str) { strRelease = str; } }
    const char* getDistribution() { return strDistrib.c_str(); }
    const char* getRelease() { return strRelease.c_str(); }

protected:

private:
    std::string strDistrib;
    std::string strRelease;
    bool satisfy_platform(Platform* os);

};


class ResYarpPort : public GenericResource
{
public:
    ResYarpPort();
    ResYarpPort(const char* szName);
    ~ResYarpPort() override;
    void setPort(const char* szPort) { if(szPort) { strPort = szPort; } }
    const char* getPort() { return strPort.c_str(); }
    void setRequest(const char* szReq) { if(szReq) { strRequest = szReq; } }
    void setReply(const char* szRep) { if(szRep) { strReply = szRep; } }
    const char* getRequest() { return strRequest.c_str(); }
    const char* getReply() { return strReply.c_str(); }
    void setTimeout(double t) { timeout = t; }
    double getTimeout() { return timeout; }
    Node* clone() override;
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
