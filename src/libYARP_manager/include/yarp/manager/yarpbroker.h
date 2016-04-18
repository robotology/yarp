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

using namespace std;
//using namespace yarp::os;

namespace yarp {
namespace manager {


/**
 * Class Broker
 */
class YarpBroker: public Broker, public yarp::os::RateThread {

public:
    YarpBroker();
     virtual ~YarpBroker();
     bool init();
     bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv );
     void fini(void);
     bool start();
     bool stop();
     bool kill();
     bool connect(const char* from, const char* to,
                        const char* carrier, bool persist=false);
     bool disconnect(const char* from, const char* to);
     bool rmconnect(const char* from, const char* to);
     int running(void);
     bool exists(const char* port);
     const char* requestRpc(const char* szport, const char* request, double timeout);
     bool connected(const char* from, const char* to);
     const char* error(void);
     bool initialized(void) { return bInitialized;}
     bool attachStdout(void);
     void detachStdout(void);

     bool getSystemInfo(const char* server,
                        yarp::os::SystemInfoSerializer& info);
     bool getAllProcesses(const char* server,
                        ProcessContainer &processes);
     bool getAllPorts(vector<std::string> &stingList);

     bool setQos(const char* from, const char* to,
                 const char* qosFrom, const char* qosTo);

public: // for rate thread
    void run();
    bool threadInit();
    void threadRelease();

protected:

private:
    string strCmd;
    string strParam;
    string strHost;
    string strStdio;
    string strWorkdir;
    string strTag;
    string strEnv;
    unsigned int ID;
    yarp::os::Property command;
    string strError;
    bool bOnlyConnector;
    bool bInitialized;
    yarp::os::Semaphore semParam;
    string strStdioUUID;
    string __trace_message;

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
