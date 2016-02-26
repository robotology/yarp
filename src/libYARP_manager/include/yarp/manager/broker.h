/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_MANAGER_BROKER
#define YARP_MANAGER_BROKER

#include <string>
#include <vector>

#include <yarp/manager/ymm-types.h>

using namespace std;

namespace yarp {
namespace manager {


class BrokerEventSink
{
public:
    virtual ~BrokerEventSink() {}
    virtual void onBrokerStdout(const char* msg) {}
    virtual void onBrokerModuleFailed(void) {}
};


/**
 * Class Broker
 */
class Broker {

public:
    Broker(void);
    virtual ~Broker();
    void setEventSink(BrokerEventSink* pEventSink);
    virtual bool init(void) = 0; //only connector
    virtual bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv ) = 0;
    virtual void fini(void) = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool kill() = 0;
    virtual bool connect(const char* from, const char* to,
                        const char* carrier, bool persist=false) = 0;
    virtual bool disconnect(const char* from, const char* to) = 0;
    virtual int  running(void) = 0; // 0 if is not running and 1 if is running; otherwise -1.
    virtual bool exists(const char* port) = 0;
    virtual const char* requestRpc(const char* szport, const char* request, double timeout=0.0) = 0;
    virtual bool connected(const char* from, const char* to) = 0;
    virtual const char* error(void) = 0;
    virtual bool initialized(void) = 0;
    virtual bool attachStdout(void) = 0;
    virtual void detachStdout(void) = 0;

    unsigned int generateID(void);

    void enableWatchDog(void) { bWithWatchDog = true; }
    void disableWatchDog(void) { bWithWatchDog = false; }
    bool hasWatchDog(void) { return bWithWatchDog; }    
    void setDisplay(const char* szDisplay) {if(szDisplay) strDisplay = szDisplay;}

    const char* getDisplay() const {return strDisplay.c_str(); }
protected:
    unsigned int UNIQUEID;
    BrokerEventSink* eventSink;
    bool bWithWatchDog;
    string strDisplay;
    
private:

};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_BROKER__
