/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
     bool init() YARP_OVERRIDE;
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv ) YARP_OVERRIDE;
     void fini(void) YARP_OVERRIDE;
     bool start() YARP_OVERRIDE;
     bool stop() YARP_OVERRIDE;
     bool kill() YARP_OVERRIDE;
     bool connect(const char* from, const char* to,
                        const char* carrier, bool persist=false) YARP_OVERRIDE;
     bool disconnect(const char* from, const char* to, const char* carrier) YARP_OVERRIDE;
     bool rmconnect(const char* from, const char* to);
     int running(void) YARP_OVERRIDE;
     bool exists(const char* port) YARP_OVERRIDE;
     const char* requestRpc(const char* szport, const char* request, double timeout) YARP_OVERRIDE;
     bool connected(const char* from, const char* to, const char* carrier) YARP_OVERRIDE;
     const char* error(void) YARP_OVERRIDE;
     bool initialized(void) YARP_OVERRIDE { return bInitialized;}
     bool attachStdout(void) YARP_OVERRIDE;
     void detachStdout(void) YARP_OVERRIDE;

     bool getSystemInfo(const char* server,
                        yarp::os::SystemInfoSerializer& info);
     bool getAllProcesses(const char* server,
                        ProcessContainer &processes);
     bool getAllPorts(std::vector<std::string> &stingList);

     bool setQos(const char* from, const char* to,
                 const char* qosFrom, const char* qosTo);

public: // for rate thread
    void run() YARP_OVERRIDE;
    bool threadInit() YARP_OVERRIDE;
    void threadRelease() YARP_OVERRIDE;

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
