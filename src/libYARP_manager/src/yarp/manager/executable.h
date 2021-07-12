/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_Executable
#define YARP_MANAGER_Executable

#include <string>
#include <vector>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/broker.h>
#include <yarp/manager/module.h>
#include <yarp/manager/application.h>
#include <yarp/manager/execstate.h>


namespace yarp {
namespace manager {


#define DEF_PERIOD      0.1  //s
#define WDOG_PERIOD     5.0 //s

typedef enum __RSTATE {
    SUSPENDED,
    READY,
    CONNECTING,
    RUNNING,
    DYING,
    DEAD,
    STUNKNOWN
} RSTATE;

enum class BrokerType
{
    invalid,
    local,
    yarp
};


class MEvent{

public:
    MEvent() {}
    virtual ~MEvent() = default;
    virtual void onExecutableStart(void* which) {}
    virtual void onExecutableStop(void* which) {}
    virtual void onExecutableDied(void* which) {}
    virtual void onExecutableFailed(void* which) {}
    virtual void onExecutableStdout(void* which, const char* msg) {}
    virtual void onCnnStablished(void* which) {}
    virtual void onCnnReleased(void* which) {}
    virtual void onCnnFailed(void* which) {}
    virtual void onError(void* which) {}
};


class ConcurentWrapper;
class ConcurentRateWrapper;

/**
 * Class Executable
 */
class Executable : public BrokerEventSink
{
public:
    Executable(Broker* _broker, MEvent* _event, Module* module, bool bWatchDog=true);
    ~Executable() override;

    bool start();
    void stop();
    void kill();

    void setID(int id) { theID = id;}
    void setCommand(const char* val) { if(val) { strCommand = val; } }
    void setParam(const char* val) { if(val) { strParam = val; } }
    void setHost(const char* val) { if(val) { strHost = val; } }
    void setStdio(const char* val) { if(val) { strStdio = val; } }
    void setWorkDir(const char* val) { if(val) { strWorkdir = val; } }
    void setEnv(const char* val) { if(val) { strEnv = val; } }

    void addConnection(Connection &cnn) { connections.push_back(cnn); }
    CnnContainer& getConnections() { return connections;}
    void addResource(ResYarpPort &res) { resources.push_back(res); }
    ResourceContainer& getResources() { return resources; }

    RSTATE state();
    BrokerType getBrokerType();
    bool shouldChangeBroker();
    Broker* getBroker() { return broker; }
    void setAndInitializeBroker(Broker* _broker);
    void removeBroker() { delete broker;}

    MEvent* getEvent() { return event; }
    const char* getCommand() { return strCommand.c_str(); }
    const char* getParam() { return strParam.c_str(); }
    const char* getHost() { return strHost.c_str(); }
    const char* getStdio() { return strStdio.c_str(); }
    const char* getWorkDir() { return strWorkdir.c_str(); }
    const char* getEnv() { return strEnv.c_str(); }
    int getID() { return theID; }
    Module* getModule() { return module; }

    void setPostExecWait(double t) { waitStart = t; }
    double getPostExecWait() { return waitStart; }
    void setPostStopWait(double t) { waitStop = t; }
    double getPostStopWait() { return waitStop; }

    void setOriginalPostExecWait(double t){ originalWaitStart = t; }
    void restoreOriginalPostExecWait(){ waitStart = originalWaitStart; }
    void setOriginalPostStopWait(double t){ originalWaitStop = t; }
    void restoreOriginalPostStopWait(){ waitStop = originalWaitStop; }

    void enableAutoConnect() { bAutoConnect = true; }
    void disableAutoConnect() { bAutoConnect = false; }
    bool autoConnect() { return bAutoConnect; }

    bool startWatchDog();
    void stopWatchDog();

public: // from BrokerEventSink
    void onBrokerStdout(const char* msg) override;

private:
    bool bAutoConnect;
    std::string strCommand;
    std::string strParam;
    std::string strHost;
    std::string strStdio;
    std::string strWorkdir;
    std::string strEnv;
    int theID;
    double waitStart;
    double waitStop;
    double originalWaitStart;
    double originalWaitStop;
    bool bWatchDog;
    Broker* broker;
    MEvent* event;
    Module* module;
    CnnContainer connections;
    ResourceContainer resources;

    ExecMachine* execMachine;
    ErrorLogger* logger;
    ConcurentWrapper* startWrapper;
    ConcurentWrapper* stopWrapper;
    ConcurentWrapper* killWrapper;
    ConcurentRateWrapper* watchdogWrapper;
    yarp::os::Semaphore semInitialize;

    void startImplement();
    void stopImplement();
    void killImplement();
    void watchdogImplement();
    bool initialize();
};


typedef std::vector<Executable*> ExecutablePContainer;
typedef std::vector<Executable*>::iterator ExecutablePIterator;
typedef void (Executable::*ExecutableFuncPtr)();

class ConcurentWrapper : public yarp::os::Thread
{
public:
    ConcurentWrapper(Executable* ptrExecutable, ExecutableFuncPtr ptrLabor)
    : labor(ptrLabor), executable(ptrExecutable) { }

    ~ConcurentWrapper() override { if(isRunning()) { stop(); } }


    void run() override {
        if(labor && executable) {
            (executable->*labor)();
        }
    }

    //bool threadInit();
    ///void threadRelease();

private:
    ExecutableFuncPtr labor;
    Executable* executable;
};


class ConcurentRateWrapper: public yarp::os::PeriodicThread
{
public:

    ConcurentRateWrapper(Executable* ptrExecutable, ExecutableFuncPtr ptrLabor)
    : PeriodicThread(WDOG_PERIOD), labor(ptrLabor), executable(ptrExecutable) { }

    ~ConcurentRateWrapper() override { if(isRunning()) { stop(); } }


    void run() override {
        if(labor && executable) {
            (executable->*labor)();
        }
    }

    //bool threadInit();
    ///void threadRelease();

private:
    ExecutableFuncPtr labor;
    Executable* executable;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_Executable__
