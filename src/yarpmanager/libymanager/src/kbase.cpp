/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <cctype>
#include <string>
#include <string.h>
#include <fstream>
#include <map> 
#include <algorithm>

#include "kbase.h"
#include "utility.h"
#include "resource.h"

using namespace std; 


#define YARP_NAME		"YARP"
#define YARP_PORT		"/root"

bool KnowledgeBase::createFrom(ModuleLoader* _mloader, 
							   AppLoader* _apploader)
{
	//__CHECK_NULLPTR(_mloader);
	//__CHECK_NULLPTR(_apploader);
	modloader = _mloader;
	apploader = _apploader;
	
	ErrorLogger* logger  = ErrorLogger::Instance();
	
	/**
	 * Clearing graph 
	 */ 
	kbGraph.clear();
	tmpGraph.clear();
	selnodes.clear();
	selconnections.clear();
	selmodules.clear();	
	selresources.clear();

	/**
	 * Adding YARP root resource to the graph
	 * TODO: notice that later it should be loaded automatically 
	 *		 from a resource xml file.
	 */
	ResYarpPort yroot(YARP_NAME);
	yroot.setPort(YARP_PORT);
	yroot.setLabel(YARP_NAME);
	kbGraph.addNode(&yroot);

	if(modloader)
	{
		/**
		 * Loading modules and adding them to the graph
		 */ 
		Module* module;
		modloader->reset();
		bool bAllComplete = true;
		while((module=modloader->getNextModule()))
		{
			module->setLabel(module->getName());		
			if(!addModuleToGraph(kbGraph, module))
				bAllComplete = false;
		} 
			
		if(!bAllComplete)
			logger->addWarning("Some incomplete or identical modules are ignored.");
	}
	
	if(apploader)
	{
		/**
		 * Loading application 
		 */ 
		Application* application;
		while((application = apploader->getNextApplication()))
		{
            addApplication(application);  
			//application->setLabel(application->getName());
			//application = (Application*) kbGraph.addNode(application);
		}

	}
	
	/**
	 *  updating nodes links 
	*/
	updateNodesLink(kbGraph, NODELINK_DEEP); 

	return true;
}


bool KnowledgeBase::addApplication(Application* app)
{
	__CHECK_NULLPTR(app);
	ErrorLogger* logger  = ErrorLogger::Instance();
    static unsigned int appID = 0;
	app->setLabel(app->getName());
	if(kbGraph.hasNode(app))
	{
        ostringstream newlable;
        newlable<<app->getLabel()<<"_"<<appID++; 		
        ostringstream msg;
		msg<<app->getName()<<" from "<<app->getXmlFile()<<" already exists.";
		logger->addWarning(msg);
        app->setName(newlable.str().c_str());
        app->setLabel(newlable.str().c_str());
		//return false; 
	}
	
	if(!kbGraph.addNode(app))
	{		
		ostringstream msg;
		msg<<"Application "<<app->getName()<<" cannot be added to the graph.";
		logger->addError(msg);
		return false; 
	}
	return true;
}


bool KnowledgeBase::addModule(Module* mod)
{
	__CHECK_NULLPTR(mod);

	ErrorLogger* logger  = ErrorLogger::Instance();
	mod->setLabel(mod->getName());		
	if(kbGraph.hasNode(mod))
	{
		ostringstream msg;
		msg<<"Module "<<mod->getName()<<" already exists.";
		logger->addWarning(msg);
		return false; 
	}
	if(!addModuleToGraph(kbGraph, mod))
	{
		ostringstream msg;
		msg<<"Module "<<mod->getName()<<" cannot be added to the graph.";
		logger->addError(msg);
		return false; 
	}
	updateNodesLink(kbGraph, NODELINK_DEEP);
	return true;
}


const ApplicaitonPContainer& KnowledgeBase::getApplications(void)
{
	static ApplicaitonPContainer applications; 
	applications.clear();
	for(GraphIterator itr=kbGraph.begin(); itr!=kbGraph.end(); itr++)	
		if((*itr)->getType() == APPLICATION &&
			compareString(((Application*)(*itr))->getName(),
						  ((Application*)(*itr))->getLabel()) )
			applications.push_back((Application*)(*itr));
	sort(applications.begin(), applications.end(), sortApplication());
	return applications;
}


const ModulePContainer& KnowledgeBase::getModules(void)
{
	static ModulePContainer modules; 
	modules.clear();
	for(GraphIterator itr=kbGraph.begin(); itr!=kbGraph.end(); itr++)	
		if((*itr)->getType() == MODULE)
		if((*itr)->getType() == MODULE&&
			compareString(((Module*)(*itr))->getName(),
						  ((Module*)(*itr))->getLabel()) )
			modules.push_back((Module*)(*itr));
	sort(modules.begin(), modules.end(), sortModules());
	return modules;
}

const InputContainer& KnowledgeBase::getInputCandidates(OutputData* output)
{
	static InputContainer inputs;
	inputs.clear();
	for(GraphIterator itr=kbGraph.begin(); itr!=kbGraph.end(); itr++)
		if((*itr)->getType() == INPUTD &&
			(*itr)->hasSuc(output))
			inputs.push_back(*((InputData*)(*itr)));
	return inputs;
}


const OutputContainer& KnowledgeBase::getOutputCandidates(InputData* input)
{
	static OutputContainer outputs;
	outputs.clear();
	for(int i=0; i<input->sucCount(); i++)
		outputs.push_back(*((OutputData*)input->getLinkAt(i).to()));
	return outputs;
}


bool KnowledgeBase::makeupApplication(Application* application, 
								CnnContainer* connections, 
								ResourcePContainer* resources)
{
	ErrorLogger* logger  = ErrorLogger::Instance();
	if(!application || !connections || !resources)
		return false;

	/**
	 * we need to load all embedded applications first
	 */
	map<string, int> appList;
	for(int i=0; i<application->iapplicationCount(); i++)
	{
		ApplicationInterface app = application->getIapplicationAt(i);
		if(string(app.getName()) == string(application->getName()))
		{
			ostringstream msg;
			msg<<"Application "<<app.getName()<<" cannot be called from itself.";
			logger->addWarning(msg);
		}
		else
		{
			Application* repapp = (Application*)kbGraph.getNode(app.getName());
			if(repapp)
			{
				if(appList.find(string(app.getName()))==appList.end())
					appList[app.getName()] = 1;
				ostringstream newname;
				newname<<application->getName()<<":"<<app.getName()<<":"<<appList[app.getName()];
				repapp = replicateApplication(tmpGraph, repapp, 
											 newname.str().c_str());
				// adding applicattion prefix to embedded applications 
				if( strlen(application->getPrefix()) )
				{
					string strPrefix = string(application->getPrefix()) + 
									   string(app.getPrefix());		
					app.setPrefix(strPrefix.c_str());
				}
				updateApplication(tmpGraph, repapp, &app);
				appList[app.getName()] = appList[app.getName()] + 1;
				//Adding embedded application as an successor to the application 
				tmpGraph.addLink(application, repapp, 0, false); 
				makeupApplication(repapp, connections, resources);
			}
			else
			{
				ostringstream msg;
				msg<<"Application "<<app.getName()<<" does not exist.";
				logger->addWarning(msg);
			}	
		}
	}


	/**
	 * loading modules
	 */
	map<string, int> modList;
	for(int i=0; i<application->imoduleCount(); i++)
	{
		Module* module;
		ModuleInterface mod = application->getImoduleAt(i);
		
		if(modList.find(string(mod.getName()))==modList.end())
			modList[mod.getName()] = 1;
		
		ostringstream newname;
		newname<<application->getLabel()<<":"<<mod.getName()<<":"<<modList[mod.getName()];
		
		Module* repmod = (Module*)kbGraph.getNode(mod.getName());
		if(repmod)
			module = replicateModule(tmpGraph, repmod, newname.str().c_str());
		else
		{
			Module newmod(mod.getName());
			newmod.setLabel(newname.str().c_str());
			module = addModuleToGraph(tmpGraph, &newmod);
			ostringstream msg;
			msg<<"Module "<<mod.getName()<<" does not exist.";
			logger->addWarning(msg);
		}

		modList[mod.getName()] = modList[mod.getName()] + 1;
		// adding application prefix to module prefix
		if( strlen(application->getPrefix()) )
		{
			string strPrefix = string(application->getPrefix()) + 
							   string(mod.getPrefix());		
			mod.setPrefix(strPrefix.c_str());
		}
		updateModule(tmpGraph, module, &mod);
				
		//Adding the module as an successor to the application 
		tmpGraph.addLink(application, module, 0, false); 
		
	} // end of for loop


	/**
	 * adding resouces dependencies to tmpGraph and resources list
	 */
	for(int i=0; i<application->resourcesCount(); i++)
	{
		
		ResYarpPort res = application->getResourceAt(i);
		res.setLabel(res.getPort());
		//cout<<res.getLabel()<<endl;
		ResYarpPort* newres = (ResYarpPort*) tmpGraph.addNode(&res);

		if(newres && 
			(find(resources->begin(), resources->end(), newres) 
			== resources->end()))
			resources->push_back(newres);
	}

	/*
	 * updating internal connections with application prefix and 
	 * adding them to connections list
	 */
	for(int i=0; i<application->connectionCount(); i++)
	{
		Connection cnn = application->getConnectionAt(i);
		if(!cnn.isExternalFrom() /*&&
			!isExternalResource(tmpGraph, cnn.from())*/)
		{
			string strPort = string(application->getPrefix()) + 
							 string(cnn.from()); 
			cnn.setFrom(strPort.c_str());
		}
		
		if(!cnn.isExternalTo() /*&&
			!isExternalResource(tmpGraph, cnn.to())*/)
		{
			string strPort = string(application->getPrefix()) + 
							 string(cnn.to()); 
			cnn.setTo(strPort.c_str());
		}
		
		if(find(connections->begin(), connections->end(), cnn) 
			== connections->end())
			connections->push_back(cnn);		
	}

	return true;
}


ResYarpPort* KnowledgeBase::findResByPort(Graph& graph, const char* szPort)
{
	for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)
	{
		if((*itr)->getType() == RESOURCE)
		{
			ResYarpPort* res = (ResYarpPort*)(*itr);		
			if(compareString(res->getPort(), szPort))
				return res;
		}
	}
	return NULL;
}


InputData* KnowledgeBase::findInputByPort(Graph& graph, const char* szPort)
{
	for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)
	{
		if((*itr)->getType() == INPUTD )
		{
			InputData* input = (InputData*)(*itr);		
			if(compareString(input->getPort(), szPort) /*&&
			   !compareString(input->getName(), input->getLabel())*/ )
				return input;
		}
	}
	return NULL;
}


OutputData* KnowledgeBase::findOutputByPort(Graph& graph, const char* szPort)
{
	for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)
	{
		if((*itr)->getType() == OUTPUTD)
		{
			OutputData* output = (OutputData*)(*itr);		
			if(compareString(output->getPort(), szPort))
				return output;
		}
	}
	return NULL;
}

bool KnowledgeBase::isExternalResource(Graph& graph, const char* szName)
{
	if(!findInputByPort(graph, szName) && 
		 !findOutputByPort(graph, szName) &&
		 !findResByPort(graph, szName))
		 return true;
	return false; 
}

/*
bool KnowledgeBase::reasolveDependency(Module* mod, bool bAutoDependancy)
{
	__CHECK_NULLPTR(mod);
	return reasolveDependency(mod->getName(), bAutoDependancy);
}
*/

bool KnowledgeBase::reasolveDependency(Application* app, bool bAutoDependancy)
{
	__CHECK_NULLPTR(app);
	return reasolveDependency(app->getName(), bAutoDependancy);
}


bool KnowledgeBase::reasolveDependency(const char* szName, bool bAutoDependancy)
{
	ErrorLogger* logger  = ErrorLogger::Instance();

	selnodes.clear();
	selconnections.clear();
	selmodules.clear();	
	selresources.clear();
	tmpGraph.clear();

	Node* node = kbGraph.getNode(szName); 
	if(!node)
	{
		ostringstream msg;
		msg<<string(szName)<<" not found.";
		logger->addError(msg.str().c_str());
		return false; 
	}

	if(node->getType() != APPLICATION)
	{
		ostringstream msg;
		msg<<string(szName)<<" is not an application.";
		logger->addError(msg.str().c_str());
		return false;
	}

	/**
	 * adding application to tmpGraph
	 */
	Application* init = replicateApplication(tmpGraph, 
											(Application*)node,
											node->getLabel());

	makeupApplication(init, &selconnections, &selresources);

	/**
	 * Adding all resources to tmpGraph
	 * TODO: resources which are not connected to any input or 
	 *		 output should not be added to tmpGraph. 
	 */
	for(GraphIterator itr=kbGraph.begin(); itr!=kbGraph.end(); itr++)
		if((*itr)->getType() == RESOURCE)
		{
			replicateResource(tmpGraph, (ResYarpPort*)(*itr),
							  (*itr)->getLabel());
		}


	// adding resources for exteranl dependencies and ports 
	CnnIterator itrC; 
	for(itrC = selconnections.begin(); itrC != selconnections.end(); itrC++)
	{
		Connection cnn = (*itrC);
		if(cnn.isExternalFrom() 
			/*|| isExternalResource(tmpGraph, cnn.from())*/ )
		{
			ResYarpPort res("unknown port");
			res.setLabel(cnn.from());
			res.setPort(cnn.from());
			ResYarpPort* newres = (ResYarpPort*) tmpGraph.addNode(&res);
			selresources.push_back(newres);
		}
		if(cnn.isExternalTo() 
			/*|| isExternalResource(tmpGraph, cnn.to())*/ ) 
		{
			ResYarpPort res("unknown port");
			res.setLabel(cnn.to());
			res.setPort(cnn.to());
			ResYarpPort* newres = (ResYarpPort*) tmpGraph.addNode(&res);
			selresources.push_back(newres);
		}
	}

	if(bAutoDependancy)
	{
		updateNodesLink(tmpGraph, NODELINK_DEEP); 

		if(!checkConsistency())
		{
			logger->addError("Knowledge base is not consistent.");
			return false; 
		}
	}
	else
		updateNodesLink(tmpGraph, NODELINK_SUPERFICIAL); 

	/*
	 * updating extra node links
	 */
	updateExtraLink(tmpGraph, &selconnections);

	tmpGraph.setSatisfied(false);
	if(!bestDependancyPath(init, &selnodes, bAutoDependancy))
	{
		logger->addError("No solution found.");
		return false; 		
	}

	NodePVIterator itr;
	for(itr=selnodes.begin(); itr!=selnodes.end(); itr++)
	{
		if((*itr)->getType() == MODULE)
		{
			if(find(selmodules.begin(), selmodules.end(), 
				(Module*)(*itr)) == selmodules.end())
				selmodules.push_back((Module*)(*itr));
		}
	}
			
	OutputData* from = NULL;
	InputData* to = NULL;
	for(itr=selnodes.begin(); itr!=selnodes.end(); itr++)
	{
		if((*itr)->getType() == INPUTD)
				to = (InputData*)(*itr);
		if((*itr)->getType() == OUTPUTD)
			from = (OutputData*)(*itr);
		if(from && to)
		{
			Connection cnn(from->getPort(), to->getPort(), 
							from->getCarrier());
			if(find(selconnections.begin(), selconnections.end(),
					cnn) == selconnections.end())
			{					
				cnn.setPriority(to->withPriority());			
				cnn.setOwner(findOwner(tmpGraph, to));
				selconnections.push_back(cnn);
			}
			from = NULL;
			to = NULL;
		}
	}

	for(itr=selnodes.begin(); itr!=selnodes.end(); itr++)
	{
		if((*itr)->getType() == RESOURCE)
		{
			if(find(selresources.begin(), selresources.end(), 
				(ResYarpPort*)(*itr)) == selresources.end())
				selresources.push_back((ResYarpPort*)(*itr));
		}
	}

	/*	
	ModulePIterator itrM;
	cout<<"Selected tasks:"<<endl;
	for(itrM=selmodules.begin(); itrM!=selmodules.end(); itrM++)
		cout<<(*itrM)->getName()<<" ("<<(*itrM)->getPrefix()<<")"<<endl;
	cout<<endl;
	
	//CnnIterator itrC;
	cout<<"Selected connections:"<<endl;
	for(itrC=selconnections.begin(); itrC!=selconnections.end(); itrC++)
		if((*itrC).owner())
			cout<<(*itrC).owner()->getLabel()<<": { "<<(*itrC).from()<<" -> "<<(*itrC).to()<<" }"<<endl;
		else
			cout<<"Extra: { "<<(*itrC).from()<<" -> "<<(*itrC).to()<<" }"<<endl;
	cout<<endl;

	cout<<"Required resources:"<<endl;
	ResourcePIterator itrS;
	for(itrS=selresources.begin(); itrS!=selresources.end(); itrS++)
		cout<<(*itrS)->getName()<<" ("<<(*itrS)->getPort()<<")"<<endl;
	cout<<endl;
	*/

	return true;
		
}


bool KnowledgeBase::updateApplication(Graph& graph, Application* app, 
							ApplicationInterface* iapp )
{
	__CHECK_NULLPTR(app);
	__CHECK_NULLPTR(iapp);
	
	//ErrorLogger* logger  = ErrorLogger::Instance();
	
	if(strlen(iapp->getPrefix()))
		app->setPrefix(iapp->getPrefix());
	return true;
}

bool KnowledgeBase::updateModule(Graph& graph, 
					Module* module, ModuleInterface* imod )
{
	__CHECK_NULLPTR(module);
	__CHECK_NULLPTR(imod);
	
	//ErrorLogger* logger  = ErrorLogger::Instance();

	if(strlen(imod->getHost()))
		module->setHost(imod->getHost());
	if(strlen(imod->getParam()))
		module->setParam(imod->getParam());
	if(imod->getRank()>0)
		module->setRank(imod->getRank());	
	if(strlen(imod->getBroker()))
		module->setBroker(imod->getBroker());
	if(strlen(imod->getStdio()))
		module->setStdio(imod->getStdio());
	if(strlen(imod->getWorkDir()))
		module->setWorkDir(imod->getWorkDir());

	// updating port's prefix
	if(strlen(imod->getPrefix()))
	{
		module->setPrefix(imod->getPrefix());
		for(int i=0; i<module->sucCount(); i++)
		{
			InputData* input = (InputData*) module->getLinkAt(i).to();
			string strPort = string(imod->getPrefix()) + string(input->getPort());
			input->setPort(strPort.c_str());
		}

		for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)	
			if((*itr)->getType() == OUTPUTD)
			{
				OutputData* output = (OutputData*)(*itr);
				Module* producer = (Module*)output->getLinkAt(0).to();
				if(producer == module)
				{
					string strPort = string(imod->getPrefix()) 
									 + string(output->getPort());
					output->setPort(strPort.c_str());
				}
			}
	}
/*
	// updating port names
	for(int i=0; i<imod->portmapCount(); i++)
	{
		Portmap port = imod->getPortmapAt(i);		
		Node* node;
		if(node = graph.getNode(createDataLabel(module->getLabel(), 
			port.oldPort(), ":I")))
				((InputData*)node)->setPort(port.newPort());
		else if(node = graph.getNode(createDataLabel(module->getLabel(), 
			port.oldPort(), ":O"))) 
				((OutputData*)node)->setPort(port.newPort());
		else
		{
			ostringstream msg;
			msg<<"Module "<<module->getName()<<" does not have port "<<port.oldPort();
			logger->addWarning(msg);			
		}
	}
*/	
	return true; 
}

/*
 * Replicate a module from graph to tmpgraph
 */
Module* KnowledgeBase::replicateModule(Graph& graph, 
						Module* module, const char* szLabel)
{
	__CHECK_NULLPTR(module);
	Module* newmod = (Module*) module->clone();
	newmod->setLabel(szLabel);
	newmod->removeAllSuc();
	if(!addModuleToGraph(graph, newmod))
	{
		delete newmod;
		return NULL;
	}
	
	delete newmod;
	return (Module*)graph.getNode(szLabel);
}


/*
 * Replicate an application from graph to tmpgraph
 */
Application* KnowledgeBase::replicateApplication(Graph& graph, 
							Application* app, const char* szLabel)
{
	__CHECK_NULLPTR(app);
	Application* newapp = (Application*) app->clone();
	newapp->setLabel(szLabel);
	newapp->removeAllSuc();
	/*Adding new application to the graph */
	Application* application = (Application*)graph.addNode(newapp);
	delete newapp;
	return application;
}

/*
 * Replicate a resource from graph to tmpgraph
 */
ResYarpPort* KnowledgeBase::replicateResource(Graph& graph, 
							ResYarpPort* res, const char* szLabel)
{
	__CHECK_NULLPTR(res);
	ResYarpPort* newres = (ResYarpPort*) res->clone();
	newres->setLabel(szLabel);
	newres->removeAllSuc();
	/*Adding new resource to the graph */
	ResYarpPort* resource = (ResYarpPort*)graph.addNode(newres);
	delete newres;
	return resource;
}


Module* KnowledgeBase::addModuleToGraph(Graph& graph, Module* module)
{
	ErrorLogger* logger  = ErrorLogger::Instance();
	
	if(!moduleCompleteness(module) && graph.hasNode(module->getName()))
		return NULL;

	/*Adding module to the graph */
	if(!(module = (Module*)graph.addNode(module)))
		return NULL;

	/* Adding inputs nodes to the graph*/
	for(int i=0; i<module->inputCount(); i++)
	{
		InputData* input = &(module->getInputAt(i));		
		input->setLabel(createDataLabel(module->getLabel(), 
										input->getPort(), ":I")); 
		
		if((input=(InputData*)graph.addNode(input)))
			graph.addLink(module, input, 0,
						!(input->isRequired()));
		else
		{
			input = &(module->getInputAt(i));
			module->removeInput(*input);
			ostringstream msg;
			msg<<"Input ";
			msg<<createDataLabel(module->getLabel(), 
								input->getPort(), ":I");
			msg<<" already exists.";
			logger->addWarning(msg);
		}
	}
	
	/* Adding output nodes to the graph*/
	for(int i=0; i<module->outputCount(); i++)
	{
		OutputData* output = &(module->getOutputAt(i));
		output->setLabel(createDataLabel(module->getLabel(),
										output->getPort(), ":O")); 
		
		if((output=(OutputData*)graph.addNode(output)))
			graph.addLink(output, module, 0);
		else
		{
			output = &(module->getOutputAt(i));
			module->removeOutput(*output);
			ostringstream msg;
			msg<<"Output ";
			msg<<createDataLabel(module->getLabel(), 
								output->getPort(), ":O");
			msg<<" already exists.";
			logger->addWarning(msg);
		}
	}
			
	return module;
}



bool KnowledgeBase::moduleCompleteness(Module* module)
{
	ErrorLogger* logger  = ErrorLogger::Instance();
	
	/* Checking module name */
	if(strlen(module->getName()) == 0)
	{
		logger->addWarning("Module has no name.");	
		return false;
	}
		
	/* Checking inputs name and port */
	for(int i=0; i<module->inputCount(); i++)
	{
		const char* szName = module->getInputAt(i).getName();
		const char* szPort = module->getInputAt(i).getPort();
		if(!strlen(szName) || !strlen(szPort))
			return false;
	}
	
	/* Checking outputs name and port */
	for(int i=0; i<module->outputCount(); i++)
	{
		const char* szName = module->getOutputAt(i).getName();
		const char* szPort = module->getOutputAt(i).getPort();
		if(!strlen(szName) || !strlen(szPort))
			return false;
	}	
	return true;
}


Module* KnowledgeBase::findOwner(Graph& graph, InputData* input)
{
	for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)
	{
		if((*itr)->getType() == MODULE)
		{
			Module* module = (Module*)(*itr);		
			for(int i=0; i<module->sucCount(); i++)
			{
				Link l = module->getLinkAt(i);
				if((InputData*)l.to() == input)
					return module;
			}
		}
	}
	return NULL;
}


void KnowledgeBase::updateExtraLink(Graph& graph, CnnContainer* connections)
{
	// update graph with external connection
	CnnIterator itrC;
	for(itrC=connections->begin(); itrC!=connections->end(); itrC++)
	{
		Connection cnn = (*itrC);
		//cout<<"connecting "<<cnn.from()<< " to "<<cnn.to()<<endl;

		ResYarpPort* res = findResByPort(graph, cnn.from());
		OutputData* output = findOutputByPort(graph, cnn.from());
		InputData* input = findInputByPort(graph, cnn.to());
		if(res && input)
		{
			res->setOwner(findOwner(graph, input));
			(*itrC).setOwner(findOwner(graph, input));
			input->addSuc(res, 0.0);
		}
		else if(output && input)
		{
			(*itrC).setOwner(findOwner(graph, input));
			input->addSuc(output, 0.0);	
		}
	/*	
		res = findResByPort(graph, cnn.to());
		output = findOutputByPort(graph, cnn.from());	
		if(res && output)
			res->addSuc(output, 0.0);
	*/
	}
}

void KnowledgeBase::updateNodesLink(Graph& graph, int level)
{
	/**
	 * link inputs to relevant outputs and resources
	 */
	for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)	
	{
		if((*itr)->getType() == INPUTD)
		{
			InputData* input = (InputData*)(*itr);
			/**
			 * adding resources to the relevant inputs
			 */
			for(GraphIterator itr2=graph.begin(); itr2!=graph.end(); itr2++)	
			{
				if((*itr2)->getType() == RESOURCE)
				{
					ResYarpPort* res = (ResYarpPort*)(*itr2);
					//Module* producer = (Module*)output->getLinkAt(0).to();			
					if(compareString(res->getName(), input->getName()))
						graph.addLink(input, res, 0.0, false);
				}
			}

			/**
			 * adding relevant outputs to inputs
			 */
			if(level==NODELINK_DEEP)
				linkToOutputs(graph, input);
		}
	}


}


void KnowledgeBase::linkToOutputs(Graph& graph, InputData* input)
{
	for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)	
	{
		if((*itr)->getType() == OUTPUTD)
		{
			OutputData* output = (OutputData*)(*itr);
			Module* producer = (Module*)output->getLinkAt(0).to();			
			if(compareString(output->getName(), input->getName())
			   &&(producer != findOwner(graph, input)))
				graph.addLink(input, output, 
							 (float)getProducerRank(graph, output),
							 !input->isRequired());
		}
	}
}

/**
 * Notice that KB is generated in such way that each OutputData is 
 * connected to only one Module.   
 */
int KnowledgeBase::getProducerRank(Graph& graph, OutputData* output)
{
	if(output->sucCount())
	{
		Module* module = (Module*)output->getLinkAt(0).to();
		return module->getRank();
	}
	return 0;
}


bool KnowledgeBase::checkConsistency(void)
{
	//ErrorLogger* logger  = ErrorLogger::Instance();

	/**
	 *  check whether there is atleast one application
	 */
	/*
	 if(!application)
	 {
		 logger->addError("No application found.");
		 return false;
	 }
	 */

	/**
	 *  check whether any modules are listed in the application 
	 *  any application should have at list one module 
	 */
	/*
	 if(application->isLeaf())
	 {
		 logger->addWarning("Empty application.");
		 //return false;
	 } 
	 */
	return true; 
}

bool KnowledgeBase::constrainSatisfied(Node* node, bool bAutoDependancy)
{
	ErrorLogger* logger  = ErrorLogger::Instance();
	
	switch(node->getType()) {
		case INPUTD: {
			if( ((InputData*)node)->isRequired() && node->isLeaf() )
			{
				ostringstream msg;
				msg<<"Unsatisfied constrain. ";
				msg<<node->getLabel()<<" has no output candidate!";
				logger->addWarning(msg);
				if(bAutoDependancy)
					return false;
			}
			break;
		}
		default:
			break;
	};
	return true;
}


bool KnowledgeBase::bestDependancyPath(Node* initial,  NodePVector* path,
									bool bAutoDependancy)
{
	 //if(initial->isSatisfied())
		//return true;
	if(!constrainSatisfied(initial, bAutoDependancy))
		return false;
	
	if(initial->isLeaf())
	{
		path->push_back(initial);
		initial->setSatisfied(true);
		initial->setVisited(false);
		return true;
	}
	
	/* now we are visiting this node*/
	initial->setVisited(true);
	bool bPathFound = false;
	
	/**
	 *  we use the following variable for data nodes
	 */
	NodePVector bestRankedPath; 
	float bestRank = 0;
	
	for(int i=0; i<initial->sucCount(); i++)
	{
		Link l = initial->getLinkAt(i);
		/* we are not interested in virtual links
		 * and we are not meeting a visited node to avoid looping
		 */
		if((l.isVirtual() == false) && (l.to())->isVisited() == false)
		{
			NodePVector subPath; 
			bool ret = bestDependancyPath(l.to(), &subPath, bAutoDependancy);
			if(ret)
			{
				bPathFound = true;
				/* we meet a conjunctive node and we need to copy all the
				 *  successors found from subpath to bestRankedPath. 
				 */
				if ((initial->getType() == APPLICATION) ||
					 (initial->getType() == MODULE))
				{
					bestRankedPath.insert(bestRankedPath.end(), 
										subPath.begin(), subPath.end());
				}
				else /* we should choose the path from highest ranked link*/
				{
					if(bestRankedPath.empty() || l.weight() > bestRank)
					{
						bestRank = l.weight();
						bestRankedPath = subPath;
					}
				}
				
			}
			else if ((initial->getType() == APPLICATION) ||
					 (initial->getType() == MODULE))
				{
					initial->setVisited(false);
					return false;
				}
		}
	}
	
	if(!bPathFound)	
	{
		initial->setVisited(false);
		return false;
	}
		
//	if((initial->getType() == INPUTD) && !bestRankedPath.empty())
//		cout<<initial->getLabel()<<" -> "<<bestRankedPath.back()->getLabel()<<endl;
	
	*path = bestRankedPath;
	//if(find(path->begin(), path->end(), initial) == path->end())
	path->push_back(initial);
	initial->setSatisfied(true);
	initial->setVisited(false);
	return true;
}

const char* KnowledgeBase::createDataLabel(const char* modlabel, 
										   const char* port,
										   const char* postfix)
{
	ostringstream name;
	name<<modlabel<<":"<<port;
	if(postfix)
		name<<postfix;
	return name.str().c_str(); 
}


/*
const char* KnowledgeBase::createOutputLabel(Module* module, OutputData* output)
{
	ostringstream name;
	name<<module->getLabel()<<":";
	name<<output->getPort()<<":";
	name<<output->getName()<<":O";	
	return name.str().c_str(); 
}
*/

/*
const char* KnowledgeBase::createAppLabel(Application* app)
{
	ostringstream name;
	name<<app->getName()<<":APP";
	return name.str().c_str(); 
}
*/


bool KnowledgeBase::exportDotGraph(Graph& graph, const char* szFileName)
{
	ofstream dot; 
	dot.open(szFileName);
	if(!dot.is_open())
		return false;
	
	dot<<"digraph G {"<<endl;
	dot<<"rankdir=LR;"<<endl;
	
	for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)	
	{
		switch((*itr)->getType()) {
			case MODULE: {
					Module* mod = (Module*)(*itr);
					dot<<"\""<<mod->getLabel()<<"\"";
					dot<<" [label=\""<< mod->getName()<<"\"";
					dot<<" shape=component, color=black, fillcolor=lightslategrey, peripheries=2, style=filled];"<<endl;
					for(int i=0; i<mod->sucCount(); i++)
					{
						Link l = mod->getLinkAt(i);
						InputData* in = (InputData*)l.to();
						dot<<"\""<<mod->getLabel()<<"\" -> ";
						dot<<"\""<<in->getLabel()<<"\"";
						if(!l.isVirtual())
							dot<<" [label=\""<<l.weight()<<"\"];"<<endl;
						else
							dot<<" [label=\""<<l.weight()<<"\" style=dashed];"<<endl;
						
					}
					
					break;
				}
			case INPUTD:{
					InputData* in = (InputData*)(*itr);
					dot<<"\""<<in->getLabel()<<"\"";
					if(in->withPriority())
					{
						dot<<" [color=red, fillcolor=thistle, peripheries=1, style=filled";
						dot<<" label=\""<< in->getName()<<"\\n"<<in->getPort()<<"\"];"<<endl;
					}
					else
					{
						dot<<" [color=black, fillcolor=thistle, peripheries=1, style=filled";
						dot<<" label=\""<< in->getName()<<"\\n"<<in->getPort()<<"\"];"<<endl;
					}
					for(int i=0; i<in->sucCount(); i++)
					{
						Link l = in->getLinkAt(i);
						OutputData* out = (OutputData*)l.to();
						dot<<"\""<<in->getLabel()<<"\" -> ";
						dot<<"\""<<out->getLabel()<<"\"";
						if(!l.isVirtual())
							dot<<" [label=\""<<l.weight()<<"\"];"<<endl;
						else
							dot<<" [label=\""<<l.weight()<<"\" style=dashed];"<<endl;
					}
					
					break;
				}				
			case OUTPUTD:{
					OutputData* out = (OutputData*)(*itr);
					dot<<"\""<<out->getLabel()<<"\"";
					dot<<" [color=black, fillcolor=wheat, peripheries=1, style=filled";
					dot<<" label=\""<< out->getName()<<"\\n"<<out->getPort()<<"\"];"<<endl;
					for(int i=0; i<out->sucCount(); i++)
					{
						Link l = out->getLinkAt(i);
						Module* mod = (Module*)l.to();
						dot<<"\""<<out->getLabel()<<"\" -> ";
						dot<<"\""<<mod->getLabel()<<"\"";
						dot<<" [label=\""<<l.weight()<<"\"];"<<endl;
					}
					
					break;
				}
			
			case APPLICATION:{
					Application* app = (Application*)(*itr);
					dot<<"\""<<app->getLabel()<<"\"";
					dot<<" [shape=box, color=black, fillcolor=darkseagreen, peripheries=2, style=filled ";
					dot<<" label=\""<<app->getLabel()<<"\""<<"];"<<endl;					
					for(int i=0; i<app->sucCount(); i++)
					{
						Link l = app->getLinkAt(i);
						Module* mod = (Module*)l.to();
						dot<<"\""<<app->getLabel()<<"\" -> ";
						dot<<"\""<<mod->getLabel()<<"\"";
						dot<<" [label=\""<<l.weight()<<"\"];"<<endl;
					}
					break;
			}
			
			case RESOURCE:{
					ResYarpPort* res = (ResYarpPort*)(*itr);
					dot<<"\""<<res->getLabel()<<"\"";
					dot<<" [shape=house, color=black, fillcolor=indianred, peripheries=2, style=filled ";
					dot<<" label=\""<<res->getPort()<<"\""<<"];"<<endl;
					break;
			}
		
			default:
				break;
		};
	}
	
	//dot<<"A "<<"[peripheries=2, color=\"gray\", style=filled];"<<endl;
	//dot<<"B [shape=box, color=\"1.0 .5 .8\", peripheries=2, style=filled];"<<endl;
	//dot<<"A -> B [label=\"3\"]"<<endl;
	
	dot<<"}"<<endl;
	dot.close();
	return true;
}


/*
OutputData* KnowledgeBase::selectBestOutput(const char* szDataName)
{
	OutputData* best = NULL;
	int iMaxRank = 0;
	for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)	
		if((*itr)->getType() == OUTPUTD)
		{
			OutputData* output = (OutputData*)(*itr);
			if(compareString(output->getName(), szDataName))
			{
				cout<<output->getLabel()<<endl;
				int thisRank = getProducerRank(output);
				if( (thisRank > iMaxRank) || !best)
				{
					best = output;
					iMaxRank = thisRank; 
				}
			}
		}
	return best;
}
*/


