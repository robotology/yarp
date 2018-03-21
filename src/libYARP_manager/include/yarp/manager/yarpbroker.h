/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_YARPBROKER
#define YARP_MANAGER_YARPBROKER

#include <string>
#include <iostream>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/RateThread.h>

// FIXME Do not use yarp/os/impl in .h files
#include <yarp/os/SystemInfo.h>
#include <yarp/os/SystemInfoSerializer.h>

#include <yarp/manager/broker.h>
#include <yarp/manager/primresource.h>

namespace yarp {
namespace manager {


/**
 * Class Broker
 */
class YarpBroker: public Broker, public yarp::os::RateThread {

public:
    YarpBroker();
     virtual ~YarpBroker();
     bool init() override;
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv ) override;
     void fini(void) override;
     bool start() override;
     bool stop() override;
     bool kill() override;
     bool connect(const char* from, const char* to,
                        const char* carrier, bool persist=false) override;
     bool disconnect(const char* from, const char* to, const char* carrier) override;
     bool rmconnect(const char* from, const char* to);
     int running(void) override;
     bool exists(const char* port) override;
     const char* requestRpc(const char* szport, const char* request, double timeout) override;
     bool connected(const char* from, const char* to, const char* carrier) override;
     const char* error(void) override;
     bool initialized(void) override { return bInitialized;}
     bool attachStdout(void) override;
     void detachStdout(void) override;

     bool getSystemInfo(const char* server,
                        yarp::os::SystemInfoSerializer& info);
     bool getAllProcesses(const char* server,
                        ProcessContainer &processes);
     bool getAllPorts(std::vector<std::string> &stingList);

     bool setQos(const char* from, const char* to,
                 const char* qosFrom, const char* qosTo);

public: // for rate thread
    void run() override;
    bool threadInit() override;
    void threadRelease() override;

protected:

private:
    std::string strCmd;
    std::string strParam;
    std::string strHost;
    std::string strStdio;
    std::string strWorkdir;
    std::string strTag;
    std::string strEnv;
    unsigned int ID;
    yarp::os::Property command;
    std::string strError;
    bool bOnlyConnector;
    bool bInitialized;
    yarp::os::Semaphore semParam;
    std::string strStdioUUID;
    std::string __trace_message;

    yarp::os::BufferedPort<yarp::os::Bottle> stdioPort;
    //yarp::os::Port port;

    bool timeout(double base, double timeout);
    yarp::os::Property& runProperty(void);
    int requestServer(yarp::os::Property& config);
    int SendMsg(yarp::os::Bottle& msg, yarp::os::ConstString target,
                yarp::os::Bottle& resp, float fTimeout=5.0);
    bool getQosFromString(const char* qos, yarp::os::QosStyle& style);

};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_YARPBROKER__
