/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_BROKER
#define YARP_MANAGER_BROKER

#include <string>
#include <vector>

#include <yarp/manager/ymm-types.h>


namespace yarp {
namespace manager {


class BrokerEventSink
{
public:
    virtual ~BrokerEventSink() {}
    virtual void onBrokerStdout(const char* msg) {}
    virtual void onBrokerModuleFailed() {}
};


/**
 * Class Broker
 */
class Broker {

public:
    Broker();
    virtual ~Broker();
    void setEventSink(BrokerEventSink* pEventSink);
    virtual bool init() = 0; //only connector
    virtual bool init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv ) = 0;
    virtual void fini() = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool kill() = 0;
    virtual bool connect(const char* from, const char* to,
                        const char* carrier, bool persist=false) = 0;
    virtual bool disconnect(const char* from, const char* to,
                            const char* carrier) = 0;
    virtual int  running() = 0; // 0 if is not running and 1 if is running; otherwise -1.
    virtual bool exists(const char* port) = 0;
    virtual const char* requestRpc(const char* szport, const char* request, double timeout=0.0) = 0;
    virtual bool connected(const char* from, const char* to,
                           const char* carrier) = 0;
    virtual const char* error() = 0;
    virtual bool initialized() = 0;
    virtual bool attachStdout() = 0;
    virtual void detachStdout() = 0;

    unsigned int generateID();

    void enableWatchDog() { bWithWatchDog = true; }
    void disableWatchDog() { bWithWatchDog = false; }
    bool hasWatchDog() { return bWithWatchDog; }
    void setDisplay(const char* szDisplay) { if(szDisplay) { strDisplay = szDisplay; } }

    const char* getDisplay() const {return strDisplay.c_str(); }
protected:
    unsigned int UNIQUEID;
    BrokerEventSink* eventSink;
    bool bWithWatchDog;
    std::string strDisplay;

private:

};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_BROKER__
