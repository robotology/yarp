// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __KBASE__
#define __KBASE__

#include "ymm-types.h" 
#include "graph.h"
#include "modloader.h"
#include "apploader.h"
#include "resource.h"
#include <string.h>

using namespace std; 

//namespace ymm {

typedef vector<Node*> NodePVector;
typedef vector<Node*>::iterator NodePVIterator;


typedef vector<Module*> ModulePContainer;
typedef vector<Module*>::iterator ModulePIterator;
typedef vector<Application*> ApplicaitonPContainer;
typedef vector<Application*>::iterator ApplicationPIterator;
typedef vector<ResYarpPort*> ResourcePContainer;
typedef vector<ResYarpPort*>::iterator ResourcePIterator;


#define NODELINK_SUPERFICIAL	1
#define NODELINK_DEEP			2



/**
 * Class KnowledgeBase  
 */
class KnowledgeBase{

public:
	KnowledgeBase(void){};
	virtual ~KnowledgeBase() { 
		kbGraph.clear(); 
		tmpGraph.clear();
	};
	
	bool createFrom(ModuleLoader* _mloader, AppLoader* _apploader);
	bool addApplication(Application* app);
	bool addModule(Module* mod);
	bool removeApplication(Application* app);

	bool reasolveDependency(const char* szName, bool bAutoDependancy=false);
	bool reasolveDependency(Application* app, bool bAutoDependancy=false);
	//bool reasolveDependency(Module* mod, bool bAutoDependancy=false);

	const ModulePContainer& getSelModules(void) { return selmodules; }
	const CnnContainer& getSelConnection(void) { return selconnections; }
	const ResourcePContainer& getSelResources(void) { return selresources; }

	const ApplicaitonPContainer& getApplications(void);
	const ModulePContainer& getModules(void);
	Module* getModule(const char* szName) { 
				return (Module*) kbGraph.getNode(szName); 
				}
	Application* getApplication(const char* szName) { 
				return (Application*) kbGraph.getNode(szName); 
				}
	const InputContainer& getInputCandidates(OutputData* output);
	const OutputContainer& getOutputCandidates(InputData* input);

	bool exportAppGraph(const char* szFileName) {
			return exportDotGraph(tmpGraph, szFileName); }
	
	bool exportKbGraph(const char* szFileName) {
			return exportDotGraph(kbGraph, szFileName);
	}


	bool checkConsistency(void);

protected:

private:
	Graph kbGraph;
	Graph tmpGraph; 
	//Application* application;
	ModuleLoader* modloader;
	AppLoader* apploader;
	NodePVector selnodes; 
	ModulePContainer selmodules;
	ApplicaitonPContainer applications; 
	CnnContainer selconnections;
	ResourcePContainer selresources; 

	bool moduleCompleteness(Module* module);
	void updateNodesLink(Graph& graph, int level);
	void updateExtraLink(Graph& graph, CnnContainer* connections);
	void linkToOutputs(Graph& graph, InputData* input);
	int getProducerRank(Graph& graph, OutputData* output);
	const char* createDataLabel(const char* modlabel, const char* port,
								const char* postfix=NULL);
	const char* createAppLabel(Application* app);
	Module* replicateModule(Graph& graph, 
							Module* module, const char* szLabel);
	Application* replicateApplication(Graph& graph, 
							Application* app, const char* szLabel);
	ResYarpPort* replicateResource(Graph& graph, 
							ResYarpPort* res, const char* szLabel);

	Module* addModuleToGraph(Graph& graph, Module* module);
	bool updateModule(Graph& graph, 
					 Module* module, ModuleInterface* imod );
	bool updateApplication(Graph& graph, 
						  Application* app, ApplicationInterface* iapp);
	bool bestDependancyPath(Node* initial, 
							NodePVector* path, bool bAutoDependancy);
	bool constrainSatisfied(Node* node, bool bAutoDependancy);
	Module* findOwner(Graph& graph, InputData* input); 
	bool makeupApplication(Application* application, 
							CnnContainer* connections,
							ResourcePContainer* resources);

	OutputData* findOutputByPort(Graph& graph, const char* szPort);
	InputData* findInputByPort(Graph& graph, const char* szPort);
	ResYarpPort* findResByPort(Graph& graph, const char* szPort);
	bool isExternalResource(Graph& graph, const char* szName);

	bool exportDotGraph(Graph& graph, const char* szName);

	//OutputData* selectBestOutput(const char* szDataName);
};



class sortApplication
{
public:
     bool operator()(Application *f, Application *s)
     {
	 	return strcmp((*f).getName(), (*s).getName()) < 0;
     }
};


class sortModules
{
public:
     bool operator()(Module *f, Module *s)
     {
	 	return strcmp((*f).getName(), (*s).getName()) < 0;
     }
};


//}

#endif //__KBASE__
