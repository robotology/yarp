/*
 *  YARP Modules Manager
 *  Copyright: (C) 2014 iCub Facility - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef SAFE_MANAGER_H
#define SAFE_MANAGER_H


#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>

#include <vector>

#include <yarp/manager/manager.h>


typedef enum __ThreadAction {
    MRUN,
    MSTOP,
    MKILL,
    MCONNECT,
    MDISCONNECT,
    MREFRESH,
    MREFRESH_CNN,
    MATTACHSTDOUT,
    MDETACHSTDOUT,
    MLOADBALANCE,
    MNOTHING
} ThreadAction;


class ApplicationEvent{
public:
    ApplicationEvent() {}
    virtual ~ApplicationEvent() {}
    virtual void onModStart(int which) {}
    virtual void onModStop(int which) {}
    virtual void onModStdout(int which, const char* msg) {}
    virtual void onConConnect(int which) {}
    virtual void onConDisconnect(int which) {}
    virtual void onResAvailable(int which) {}
    virtual void onResUnAvailable(int which) {}
    virtual void onConAvailable(int from, int to) {}
    virtual void onConUnAvailable(int from, int to) {}
    virtual void onError(void) {}
    virtual void onLoadBalance(void) {}    
};



class SafeManager: public yarp::manager::Manager, yarp::os::Thread
{
public:
    SafeManager();
    virtual ~SafeManager();
    bool prepare(yarp::manager::Manager* lazy, yarp::os::Property* config, ApplicationEvent* event=NULL);
    void close();

    bool threadInit();
    void run();
    void threadRelease();

    void safeRun(std::vector<int>& MIDs);
    void safeStop(std::vector<int>& MIDs);
    void safeKill(std::vector<int>& MIDs);
    void safeConnect(std::vector<int>& CIDs);
    void safeDisconnect(std::vector<int>& CDs);
    void safeRefresh(std::vector<int>& MIDs,
                     std::vector<int>& CIDs,
                     std::vector<int>& RIDs);
    void safeAttachStdout(std::vector<int>& MIDs);
    void safeDetachStdout(std::vector<int>& MIDs);
    void safeLoadBalance(void);
    bool busy(void);

    bool checkSemaphore(void){ return semManage.check(); }
    void postSemaphore(void) { semManage.post(); }
    void waitSemaphore(void) { semManage.wait(); }


protected:
    void onExecutableStart(void* which);
    void onExecutableStop(void* which);
    void onExecutableStdout(void* which, const char* msg);
    void onCnnStablished(void* which);
    void onExecutableDied(void* which);
    void onExecutableFailed(void* which);
    void onCnnFailed(void* which);
    void onError(void* which);

private:
    yarp::os::Property* m_pConfig;    
    ThreadAction action;
    ApplicationEvent* eventReceiver;
    bool busyAction;
    yarp::os::Semaphore semManage;
    std::vector<int> modIds;
    std::vector<int> conIds;
    std::vector<int> resIds;
    void refreshPortStatus(int id);

};


#endif //_SAFE_MANAGER_H_

