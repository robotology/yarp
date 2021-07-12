/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    ~Manager() override;

    bool addApplication(const char* szFileName, char** szAppName_=nullptr, bool modifyName=false);
    bool addApplications(const char* szPath);
    bool addModule(const char* szFileName);
    bool addModules(const char* szPath);
    bool addResource(const char* szFileName);
    bool addResources(const char* szPath);

    bool removeApplication(const char* szFileName, const char* szAppName);
    bool removeModule(const char* szModName);
    bool removeResource(const char* szResName);

    bool loadApplication(const char* szAppName);
    bool saveApplication(const char* szAppName, const char* fileName=nullptr);
    bool updateExecutable(unsigned int id, const char* szparam,
                const char* szhost, const char* szstdio,
                const char* szworkdir, const char* szenv );
    bool updateConnection(unsigned int id, const char* from,
                const char* to, const char* carrier);

    Node* getNode(std::string appName);

    bool run();
    bool run(unsigned int id, bool async=false);
    bool stop();
    bool stop(unsigned int id, bool async=false);
    bool kill();
    bool kill(unsigned int id, bool async=false);
    bool connect();
    bool connect(unsigned int id);
    bool disconnect();
    bool disconnect(unsigned int id);
    bool rmconnect(unsigned int id);
    bool rmconnect();
    bool running() { return allRunning(); }
    bool running(unsigned int id);
    bool suspended() { return allStopped(); }
    bool suspended(unsigned int id);
    bool connected();
    bool connected(unsigned int id);
    bool checkDependency();
    bool exist(unsigned int id);
    bool existPortFrom(unsigned int id);
    bool existPortTo(unsigned int id);
    bool attachStdout(unsigned int id);
    bool detachStdout(unsigned int id);
    bool updateResources();
    bool updateResource(const char* szName);
    bool waitingModuleRun(unsigned int id);
    bool waitingModuleStop(unsigned int id);
    bool waitingModuleKill(unsigned int id);
    bool loadBalance();

    void setDefaultBroker(const char* szBroker) { if(szBroker) { strDefBroker = szBroker; } }
    const char* defaultBroker() { return strDefBroker.c_str(); }
    ExecutablePContainer& getExecutables() { return runnables; }
    Executable* getExecutableById(size_t id);
    bool switchBroker(size_t id);
    CnnContainer& getConnections() { return connections;}
    ResourcePContainer& getResources() { return resources; }
    const char* getApplicationName() { return strAppName.c_str(); }

    //const char* getDataPort(const char* szDataType);

    void enableRestrictedMode() { bRestricted = true; }
    void disableRestrictedMode() { bRestricted = false; }
    void enableAutoConnect() { bAutoConnect = true; }
    void disableAutoConnect() { bAutoConnect = false; }
    void enableAutoDependency() { bAutoDependancy = true; }
    void disableAutoDependency() { bAutoDependancy = false; }
    void enableWatchDog() { bWithWatchDog = true; }
    void disableWatchod() { bWithWatchDog = false; }
    bool exportDependencyGraph(const char* szFileName) {
        return knowledge.exportAppGraph(szFileName);
    }

    KnowledgeBase* getKnowledgeBase() { return &knowledge; }
    ErrorLogger* getLogger() { return logger;}

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
    void clearExecutables();
    bool isServer(Module* module);
    bool connectExtraPorts();
    bool checkPortsAvailable(Broker* broker);
    bool allRunning();
    bool oneRunning();
    bool allStopped();
    bool prepare(bool silent=true);
    bool timeout(double base, double t);
    bool updateResource(GenericResource* resource);
    Broker* createBroker(Module* module);
    bool removeBroker(Executable* exe);
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_MANAGER__
