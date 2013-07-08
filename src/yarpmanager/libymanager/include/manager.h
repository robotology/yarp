// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __MANAGER__
#define __MANAGER__

#include "ymm-types.h" 
#include "kbase.h"
#include "utility.h"
#include "executable.h"
#include "yarpbroker.h"

using namespace std; 

//namespace ymm {



/**
 * Class Manager  
 */
class Manager : public MEvent{

public: 
    Manager( bool withWatchDog=false);
    Manager(const char* szModPath, const char* szAppPath, 
            const char* szResPath, bool withWatchDog=false);
    virtual ~Manager();

    bool addApplication(const char* szFileName, char* szAppName_=NULL);
    bool addApplications(const char* szPath);
    bool addModule(const char* szFileName);
    bool addModules(const char* szPath); 
    bool addResource(const char* szFileName);
    bool addResources(const char* szPath); 
   
    bool removeApplication(const char* szAppName);
    bool removeModule(const char* szModName);
    bool removeResource(const char* szResName);

    bool loadApplication(const char* szAppName);
    bool saveApplication(const char* szAppName, const char* fileName=NULL);
    bool updateExecutable(unsigned int id, const char* szparam,
                const char* szhost, const char* szstdio,
                const char* szworkdir, const char* szenv );
    bool updateConnection(unsigned int id, const char* from,
                const char* to, const char* carrier); 
    
    bool run(void);
    bool run(unsigned int id, bool async=false);
    bool stop(void);
    bool stop(unsigned int id, bool async=false);
    bool kill(void);
    bool kill(unsigned int id, bool async=false);
    bool connect(void);
    bool connect(unsigned int id);
    bool disconnect(void);
    bool disconnect(unsigned int id);
    bool rmconnect(unsigned int id);
    bool rmconnect(void);
    bool running(void) { return allRunning(); }
    bool running(unsigned int id);
    bool suspended(void) { return allStopped(); }
    bool suspended(unsigned int id);
    bool connected(void);
    bool connected(unsigned int id);
    bool checkDependency(void);
    bool exist(unsigned int id);
    bool existPortFrom(unsigned int id);
    bool existPortTo(unsigned int id);
    bool attachStdout(unsigned int id);
    bool detachStdout(unsigned int id);
    bool updateResources(void);
    bool updateResource(const char* szName);
    bool loadBalance(void);

    void setDefaultBroker(const char* szBroker) { if(szBroker) strDefBroker = szBroker; }
    const char* defaultBroker(void) { return strDefBroker.c_str(); }
    ExecutablePContainer& getExecutables(void) { return runnables; }
    CnnContainer& getConnections(void) { return connections;}
    ResourcePContainer& getResources(void) { return resources; }
    const char* getApplicationName(void) { return strAppName.c_str(); }

    //const char* getDataPort(const char* szDataType);

    void enableRestrictedMode(void) { bRestricted = true; }
    void disableRestrictedMode(void) { bRestricted = false; }
    void enableAutoConnect(void) { bAutoConnect = true; }
    void disableAutoConnect(void) { bAutoConnect = false; }
    void enableAutoDependency(void) { bAutoDependancy = true; }
    void disableAutoDependency(void) { bAutoDependancy = false; }
    void enableWatchDog(void) { bWithWatchDog = true; }
    void disableWatchod(void) { bWithWatchDog = false; }
    bool exportDependencyGraph(const char* szFileName) { 
        return knowledge.exportAppGraph(szFileName); 
    }

    KnowledgeBase* getKnowledgeBase(void) { return &knowledge; } 
    ErrorLogger* getLogger(void) { return logger;}

protected:
    virtual void onExecutableStart(void* which);
    virtual void onExecutableStop(void* which);
    virtual void onExecutableDied(void* which);
    virtual void onExecutableFailed(void* which);
    virtual void onCnnStablished(void* which);
    virtual void onCnnFailed(void* which);
    virtual void onError(void* which);
    virtual void onExecutableStdout(void* which, const char* msg);


private:
    bool bWithWatchDog;
    bool bAutoDependancy;
    bool bAutoConnect;
    bool bRestricted;
    ErrorLogger* logger;
    string strAppName;
    string strDefBroker;
    YarpBroker connector;

    KnowledgeBase knowledge; 
    ExecutablePContainer runnables; 
    CnnContainer connections;
    ModulePContainer modules;
    ResourcePContainer resources;
    
    bool createKnowledgeBase(AppLoader &appLoader);
    void clearExecutables(void);
    bool isServer(Module* module);
    bool connectExtraPorts(void);
    bool checkPortsAvailable(Broker* broker);
    bool allRunning(void);
    bool oneRunning(void);
    bool allStopped(void);
    bool prepare(bool silent=true);
    bool timeout(double base, double timeout);
    bool updateResource(GenericResource* resource);
};
 
 
//}

#endif //__MANAGER__
