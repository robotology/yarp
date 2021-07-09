/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_KBASE
#define YARP_MANAGER_KBASE

#include <cstring>
#include <algorithm>

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/graph.h>
#include <yarp/manager/manifestloader.h>
#include <yarp/manager/resource.h>
#include <yarp/manager/logicresource.h>
#include <yarp/manager/primresource.h>

namespace yarp {
namespace manager {


#define NODELINK_SUPERFICIAL    1
#define NODELINK_DEEP           2

typedef std::vector<Node*> NodePVector;
typedef std::vector<Node*>::iterator NodePVIterator;


/**
 * Class KnowledgeBase
 */
class KnowledgeBase{

public:
    KnowledgeBase() :
        modloader(nullptr),
        apploader(nullptr),
        resloader(nullptr),
        mainApplication(nullptr)
    {}

    virtual ~KnowledgeBase() {
        kbGraph.clear();
        tmpGraph.clear();
    }

    bool createFrom(ModuleLoader* _mloader,
                    AppLoader* _apploader,
                    ResourceLoader* _resloader);
    bool addApplication(Application* application,
                        char **szAppName_=nullptr,
                        bool modifyName=false);
    bool addModule(Module* module);
    bool addResource(GenericResource* resource);
    bool removeApplication(Application* application);
    bool removeModule(Module* module);
    bool removeResource(GenericResource* resource);

    bool reasolveDependency(const char* szName,
                            bool bAutoDependancy=false, bool bSilent=false);
    bool reasolveDependency(Application* app,
                            bool bAutoDependancy=false, bool bSilent=false);
    bool checkConsistency();

    Node* getNode(std::string appName);

    const ModulePContainer& getSelModules() { return selmodules; }
    const CnnContainer& getSelConnection() { return selconnections; }
    const ResourcePContainer& getSelResources() { return selresources; }
    const ApplicaitonPContainer& getSelApplications() { return selapplications; }

    const ApplicaitonPContainer& getApplications(Application* parent=nullptr);
    const ModulePContainer& getModules(Application* parent=nullptr);
    const ResourcePContainer& getResources(Application* parent=nullptr);
    const CnnContainer& getConnections(Application* parent=nullptr);
    const ArbContainer& getArbitrators(Application* parent=nullptr);

    const InputContainer& getInputCandidates(OutputData* output);
    const OutputContainer& getOutputCandidates(InputData* input);

    Module* getModule(const char* szName) {
                return dynamic_cast<Module*>(kbGraph.getNode(szName));
                }
    Application* getApplication() { return mainApplication; }

    Application* getApplication(const char* szName) {
                return dynamic_cast<Application*>(kbGraph.getNode(szName));
                }
    GenericResource* getResource(const char* szName) {
                return dynamic_cast<GenericResource*>(kbGraph.getNode(szName));
                }

    Module* addIModuleToApplication(Application* application,
                                    ModuleInterface &mod, bool isNew=false);
    bool removeIModuleFromApplication(Application* application, const char* szTag);
    Connection& addConnectionToApplication(Application* application, Connection &cnn);
    bool removeConnectionFromApplication(Application* application, Connection &cnn);
    bool updateConnectionOfApplication(Application* application,
                                       Connection& prev, Connection& con );
    Application* addIApplicationToApplication(Application* application,
                                    ApplicationInterface &app, bool isNew=false);
    bool removeIApplicationFromApplication(Application* application, const char* szTag);

    Arbitrator& addArbitratorToApplication(Application* application, Arbitrator &arb);
    bool removeArbitratorFromApplication(Application* application, Arbitrator &arb);

    bool setModulePrefix(Module* module, const char* szPrefix, bool updateBasePrefix=true);
    bool setApplicationPrefix(Application* app, const char* szPrefix, bool updateBasePref=true);
    bool saveApplication(AppSaver* appSaver, Application* application);

    const std::string getUniqueAppID(Application* parent, const char* szAppName);

    bool exportAppGraph(const char* szFileName) {
            return exportDotGraph(tmpGraph, szFileName); }

    bool exportKbGraph(const char* szFileName) {
            return exportDotGraph(kbGraph, szFileName);
    }


protected:

private:
    Graph kbGraph;
    Graph tmpGraph;
    ModuleLoader* modloader;
    AppLoader* apploader;
    ResourceLoader* resloader;
    Application* mainApplication;

    ApplicaitonPContainer dummyApplications;
    ModulePContainer dummyModules;
    ResourcePContainer dummyResources;
    CnnContainer dummyConnections;
    ArbContainer dummyArbitrators;

    ApplicaitonPContainer selapplications;
    ModulePContainer selmodules;
    CnnContainer selconnections;
    ResourcePContainer selresources;

    std::map<std::string, int> appList;

    bool moduleCompleteness(Module* module);
    void updateNodesLink(Graph& graph, int level);
    void makeResourceLinks(Graph& graph);
    void updateResourceWeight(Graph& graph,
                              GenericResource* resource, float weight);

    //void updateExtraLink(Graph& graph, CnnContainer* connections);
    void linkToOutputs(Graph& graph, InputData* input);
    int getProducerRank(Graph& graph, OutputData* output);
    const char* createDataLabel(const char* modlabel, const char* port,
                                const char* postfix=nullptr);
    const char* createAppLabel(Application* app);
    Module* replicateModule(Graph& graph,
                            Module* module, const char* szLabel);
    Application* replicateApplication(Graph& graph,
                            Application* app, const char* szLabel);
    GenericResource* replicateResource(Graph& graph,
                            GenericResource* res, const char* szLabel);

    Module* addModuleToGraph(Graph& graph, Module* module);
    bool removeModuleFromGraph(Graph& graph, Module* mod);

    bool updateModule(Module* module, ModuleInterface* imod);
    bool updateApplication(Application* app, ApplicationInterface* iapp);
    bool reason(Graph* graph, Node* initial,
                         ApplicaitonPContainer &applications,
                         ModulePContainer &modules,
                         ResourcePContainer& resources,
                         CnnContainer &connections,
                         bool bAutoDependancy, bool bSilent);

    bool constrainSatisfied(Node* node,
                         bool bAutoDependancy, bool bSilent);
    bool makeupApplication(Application* application);
    bool isExternalResource(Graph& graph, const char* szName);
    float calculateLoad(Computer* comp);

    OutputData* findOutputByPort(Graph& graph, const char* szPort);
    InputData* findInputByPort(Graph& graph, const char* szPort);
    GenericResource* findResByName(Graph& graph, const char* szName);
    Module* findOwner(Graph& graph, InputData* input);

};



class sortApplication
{
public:
     bool operator()(Application *f, Application *s)
     {
        std::string strFirst((*f).getName());
        std::string strSecond((*s).getName());
        transform(strFirst.begin(), strFirst.end(), strFirst.begin(),
                  (int(*)(int))toupper);
        transform(strSecond.begin(), strSecond.end(), strSecond.begin(),
                  (int(*)(int))toupper);
        return (strFirst.compare(strSecond) < 0);
     }
};


class sortModules
{
public:
     bool operator()(Module *f, Module *s)
     {
        std::string strFirst((*f).getName());
        std::string strSecond((*s).getName());
        transform(strFirst.begin(), strFirst.end(), strFirst.begin(),
                  (int(*)(int))toupper);
        transform(strSecond.begin(), strSecond.end(), strSecond.begin(),
                  (int(*)(int))toupper);
        return (strFirst.compare(strSecond) < 0);
     }
};


class sortResources
{
public:
     bool operator()(GenericResource *f, GenericResource *s)
     {
        std::string strFirst(f->getName());
        std::string strSecond(s->getName());
        transform(strFirst.begin(), strFirst.end(), strFirst.begin(),
                  (int(*)(int))toupper);
        transform(strSecond.begin(), strSecond.end(), strSecond.begin(),
                  (int(*)(int))toupper);
        return (strFirst.compare(strSecond) < 0);
     }
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_KBASE__
