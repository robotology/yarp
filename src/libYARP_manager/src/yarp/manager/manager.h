/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_MANAGER
#define YARP_MANAGER_MANAGER

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/kbase.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/executable.h>
#include <yarp/manager/yarpbroker.h>

namespace yarp {
namespace manager {

/**
 * Class Manager
 */
class Manager : public MEvent{

public:
    Manager( bool withWatchDog=false);
    Manager(const char* szModPath, const char* szAppPath,
            const char* szResPath, bool withWatchDog=false);
    virtual ~Manager();

    bool addApplication(const char* szFileName, char** szAppName_=NULL, bool modifyName=false);
    bool addApplications(const char* szPath);
    bool addModule(const char* szFileName);
    bool addModules(const char* szPath);
    bool addResource(const char* szFileName);
    bool addResources(const char* szPath);

    bool removeApplication(const char* szFileName, const char* szAppName);
    bool removeModule(const char* szModName);
    bool removeResource(const char* szResName);

    bool loadApplication(const char* szAppName);
    bool saveApplication(const char* szAppName, const char* fileName=NULL);
    bool updateExecutable(unsigned int id, const char* szparam,
                const char* szhost, const char* szstdio,
                const char* szworkdir, const char* szenv );
    bool updateConnection(unsigned int id, const char* from,
                const char* to, const char* carrier);

    Node* getNode(std::string appName);

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
    bool waitingModuleRun(unsigned int id);
    bool waitingModuleStop(unsigned int id);
    bool waitingModuleKill(unsigned int id);
    bool loadBalance(void);

    void setDefaultBroker(const char* szBroker) { if(szBroker) strDefBroker = szBroker; }
    const char* defaultBroker(void) { return strDefBroker.c_str(); }
    ExecutablePContainer& getExecutables(void) { return runnables; }
    Executable* getExecutableById(size_t id);
    bool switchBroker(size_t id);
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
    void onExecutableStart(void* which) override;
    void onExecutableStop(void* which) override;
    void onExecutableDied(void* which) override;
    void onExecutableFailed(void* which) override;
    void onCnnStablished(void* which) override;
    void onCnnFailed(void* which) override;
    void onError(void* which) override;
    void onExecutableStdout(void* which, const char* msg) override;


private:
    bool bWithWatchDog;
    bool bAutoDependancy;
    bool bAutoConnect;
    bool bRestricted;
    ErrorLogger* logger;
    std::string strAppName;
    std::string strDefBroker;
    YarpBroker connector;
    std::vector<std::string> listOfXml;

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
    bool timeout(double base, double t);
    bool updateResource(GenericResource* resource);
    Broker* createBroker(Module* module);
    bool removeBroker(Executable* exe);
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_MANAGER__
