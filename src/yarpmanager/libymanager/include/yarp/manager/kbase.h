// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __KBASE__
#define __KBASE__

#include <cstring>
#include <algorithm>

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/graph.h>
#include <yarp/manager/manifestloader.h>
#include <yarp/manager/resource.h>
#include <yarp/manager/logicresource.h>
#include <yarp/manager/primresource.h>


using namespace std;

//namespace ymm {


#define NODELINK_SUPERFICIAL    1
#define NODELINK_DEEP           2

typedef vector<Node*> NodePVector;
typedef vector<Node*>::iterator NodePVIterator;


/**
 * Class KnowledgeBase
 */
class KnowledgeBase{

public:
    KnowledgeBase(void) : mainApplication(NULL) {};
    virtual ~KnowledgeBase() {
        kbGraph.clear();
        tmpGraph.clear();
    };

    bool createFrom(ModuleLoader* _mloader,
                    AppLoader* _apploader,
                    ResourceLoader* _resloader);
    bool addApplication(Application* application, char* szAppName_=NULL);
    bool addModule(Module* module);
    bool addResource(GenericResource* resource);
    bool removeApplication(Application* application);
    bool removeModule(Module* module);
    bool removeResource(GenericResource* resource);

    bool reasolveDependency(const char* szName,
                            bool bAutoDependancy=false, bool bSilent=false);
    bool reasolveDependency(Application* app,
                            bool bAutoDependancy=false, bool bSilent=false);
    bool checkConsistency(void);

    const ModulePContainer& getSelModules(void) { return selmodules; }
    const CnnContainer& getSelConnection(void) { return selconnections; }
    const ResourcePContainer& getSelResources(void) { return selresources; }
    const ApplicaitonPContainer& getSelApplications(void) { return selapplications; }

    const ApplicaitonPContainer& getApplications(Application* parent=NULL);
    const ModulePContainer& getModules(Application* parent=NULL);
    const ResourcePContainer& getResources(Application* parent=NULL);
    const CnnContainer& getConnections(Application* parent=NULL);
    const ArbContainer& getArbitrators(Application* parent=NULL);

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

    const char* getUniqueAppID(Application* parent, const char* szAppName);

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

    map<string, int> appList;

    bool moduleCompleteness(Module* module);
    void updateNodesLink(Graph& graph, int level);
    void makeResourceLinks(Graph& graph);
    void updateResourceWeight(Graph& graph,
                              GenericResource* resource, float weight);

    //void updateExtraLink(Graph& graph, CnnContainer* connections);
    void linkToOutputs(Graph& graph, InputData* input);
    int getProducerRank(Graph& graph, OutputData* output);
    const char* createDataLabel(const char* modlabel, const char* port,
                                const char* postfix=NULL);
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
        string strFirst((*f).getName());
        string strSecond((*s).getName());
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
        string strFirst((*f).getName());
        string strSecond((*s).getName());
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
        string strFirst(f->getName());
        string strSecond(s->getName());
        transform(strFirst.begin(), strFirst.end(), strFirst.begin(),
                  (int(*)(int))toupper);
        transform(strSecond.begin(), strSecond.end(), strSecond.begin(),
                  (int(*)(int))toupper);
        return (strFirst.compare(strSecond) < 0);
     }
};


//}

#endif //__KBASE__
