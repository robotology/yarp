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
#include <map>
#include <algorithm>

#include "kbase.h"
#include "utility.h"
#include "resource.h"

using namespace std;


bool KnowledgeBase::createFrom(ModuleLoader* _mloader,
                               AppLoader* _apploader, ResourceLoader* _resloader)
{
    modloader = _mloader;
    apploader = _apploader;
    resloader = _resloader;

    /**
     * Clearing graph
     */
    kbGraph.clear();
    tmpGraph.clear();
    selconnections.clear();
    selmodules.clear();
    selresources.clear();

    /*
     * Loading resources
     */
    if(resloader)
    {
        GenericResource* resource;
        resloader->reset();
        while((resource=resloader->getNextResource()))
            addResource(resource);
    }

    /**
     * Loading modules
     */
    if(modloader)
    {
        Module* module;
        modloader->reset();
        while((module=modloader->getNextModule()))
            addModule(module);
    }

    /**
     * Loading applications
     */
    if(apploader)
    {
        Application* application;
        apploader->reset();
        while((application = apploader->getNextApplication()))
            addApplication(application);
    }

    return true;
}


bool KnowledgeBase::addApplication(Application* app)
{
    __CHECK_NULLPTR(app);
    ErrorLogger* logger  = ErrorLogger::Instance();
    static map<string, int> mapId;
    app->setLabel(app->getName());
    if(kbGraph.hasNode(app))
    {
        if(mapId.find(string(app->getName()))==mapId.end())
            mapId[app->getName()] = 1;
        else
            mapId[app->getName()] = mapId[app->getName()] + 1;
        ostringstream newlable;
        newlable<<app->getLabel()<<"("<<mapId[app->getName()]<<")";
        ostringstream msg;
        msg<<app->getName()<<" from "<<app->getXmlFile()<<" already exists.";
        logger->addWarning(msg);
        app->setName(newlable.str().c_str());
        app->setLabel(newlable.str().c_str());
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
    return true;
}


bool KnowledgeBase::addResource(GenericResource* res)
{
    __CHECK_NULLPTR(res);

    // a disabled resource should not be used.
    //if(res->getDisable())
    //    return true;

    ErrorLogger* logger  = ErrorLogger::Instance();
    res->setLabel(res->getName());
    if(kbGraph.hasNode(res))
    {
        ostringstream msg;
        msg<<"Resource "<<res->getName()<<" already exists.";
        logger->addWarning(msg);
        return false;
    }
    if(!kbGraph.addNode(res))
    {
        ostringstream msg;
        msg<<"Resource "<<res->getName()<<" cannot be added to the graph.";
        logger->addError(msg);
        return false;
    }
    return true;
}


bool KnowledgeBase::removeApplication(Application* app)
{
    return kbGraph.removeNode(app);
}

bool KnowledgeBase::removeModule(Module* mod)
{
    // removing inputs and resources
    for(int i=0; i<mod->sucCount(); i++)
        kbGraph.removeNode(mod->getLinkAt(i).to());

    // removing outputs
    for(GraphIterator itr=kbGraph.begin(); itr!=kbGraph.end(); itr++)
    {
        OutputData* output = dynamic_cast<OutputData*>(*itr);
        if(output && output->sucCount() &&
           output->getLinkAt(0).to() == mod)
            kbGraph.removeNode(output);
    }
    // removing module
    return kbGraph.removeNode(mod);
}

bool KnowledgeBase::removeResource(GenericResource* res)
{
    return kbGraph.removeNode(res);
}


const ApplicaitonPContainer& KnowledgeBase::getApplications(void)
{
    dummyApplications.clear();
    for(GraphIterator itr=kbGraph.begin(); itr!=kbGraph.end(); itr++)
    {
        Application* app = dynamic_cast<Application*>(*itr);
        if(app)
            dummyApplications.push_back(app);
    }
    sort(dummyApplications.begin(), dummyApplications.end(), sortApplication());
    return dummyApplications;
}


const ModulePContainer& KnowledgeBase::getModules(void)
{
    dummyModules.clear();
    for(GraphIterator itr=kbGraph.begin(); itr!=kbGraph.end(); itr++)
    {
        Module* mod = dynamic_cast<Module*>(*itr);
        if(mod)
            dummyModules.push_back(mod);
    }
    sort(dummyModules.begin(), dummyModules.end(), sortModules());
    return dummyModules;
}

const ResourcePContainer& KnowledgeBase::getResources(void)
{
    dummyResources.clear();
    for(GraphIterator itr=kbGraph.begin(); itr!=kbGraph.end(); itr++)
    {
        Computer* res = dynamic_cast<Computer*>(*itr);
        if(res)
        {
            bool bHas = false;
            for(unsigned int i=0; i<dummyResources.size(); i++)
                if(string(dummyResources[i]->getName()) == string(res->getName()))
                {
                    bHas = true;
                    break;
                }
            if(!bHas)
                dummyResources.push_back(res);
        }
    }

    for(GraphIterator itr=tmpGraph.begin(); itr!=tmpGraph.end(); itr++)
    {
        Computer* res = dynamic_cast<Computer*>(*itr);
        if(res)
        {
            bool bHas = false;
            for(unsigned int i=0; i<dummyResources.size(); i++)
                if(string(dummyResources[i]->getName()) == string(res->getName()))
                {
                    bHas = true;
                    break;
                }
            if(!bHas)
                dummyResources.push_back(res);
        }
    }
    sort(dummyResources.begin(), dummyResources.end(), sortResources());
    return dummyResources;
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


/**
 * Recursively extend an application to its child applications and modules
 */
bool KnowledgeBase::makeupApplication(Application* application)
{
    ErrorLogger* logger  = ErrorLogger::Instance();
    if(!application)
        return false;

    /**
     * we need to load all child  applications first
     */
    map<string, int> appList;
    for(int i=0; i<application->iapplicationCount(); i++)
    {
        ApplicationInterface interfaceApp = application->getIapplicationAt(i);
        if(string(interfaceApp.getName()) == string(application->getName()))
        {
            ostringstream msg;
            msg<<"Application "<<interfaceApp.getName()<<" cannot be called from itself.";
            logger->addWarning(msg);
        }
        else
        {
            Application* repapp = dynamic_cast<Application*>(kbGraph.getNode(interfaceApp.getName()));
            if(!repapp)
            {
                ostringstream msg;
                msg<<"Application "<<interfaceApp.getName()<<" not found.";
                logger->addWarning(msg);
            }
            else
            {
                if(appList.find(string(interfaceApp.getName()))==appList.end())
                    appList[interfaceApp.getName()] = 1;
                ostringstream newname;
                newname<<application->getName()<<":";
                newname<<interfaceApp.getName()<<":"<<appList[interfaceApp.getName()];
                repapp = replicateApplication(tmpGraph, repapp,
                                             newname.str().c_str());

                // adding applicattion prefix to child application
                if( strlen(application->getPrefix()) )
                {
                    string strPrefix = string(application->getPrefix()) +
                                       string(interfaceApp.getPrefix());
                    interfaceApp.setPrefix(strPrefix.c_str());
                }

                // updating Application with ApplicationInterface
                updateApplication(tmpGraph, repapp, &interfaceApp);
                appList[interfaceApp.getName()] = appList[interfaceApp.getName()] + 1;

                //Adding child application as an successor to the application
                tmpGraph.addLink(application, repapp, 0, false);

                // recursive call to make up child application
                makeupApplication(repapp);
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

        Module* repmod = dynamic_cast<Module*>(kbGraph.getNode(mod.getName()));
        if(repmod)
            module = replicateModule(tmpGraph, repmod, newname.str().c_str());
        else
        {
            Module newmod(mod.getName());
            newmod.setLabel(newname.str().c_str());
            module = addModuleToGraph(tmpGraph, &newmod);
        }

        // adding application prefix to module prefix
        if( strlen(application->getPrefix()) )
        {
            string strPrefix = string(application->getPrefix()) +
                               string(mod.getPrefix());
            mod.setPrefix(strPrefix.c_str());
        }

        //updating Module with ModuleInterface
        updateModule(tmpGraph, module, &mod);
        modList[mod.getName()] = modList[mod.getName()] + 1;

        // adding module's resources to application resource list
        ResourceIterator itr;
        for(itr=mod.getResources().begin();
            itr!=mod.getResources().end(); itr++)
        {
            (*itr).setOwner(module);
            application->addResource(*itr);
        }
        //Adding the module as an successor to the application
        tmpGraph.addLink(application, module, 0, false);

    } // end of for loop


    /*
     * updating extera connections with application prefix
     */
    for(int i=0; i<application->connectionCount(); i++)
    {
        Connection* cnn = &application->getConnectionAt(i);
        if(!cnn->isExternalFrom())
        {
            string strPort = string(application->getPrefix()) + string(cnn->from());
            cnn->setFrom(strPort.c_str());
        }

        if(!cnn->isExternalTo())
        {
            string strPort = string(application->getPrefix()) + string(cnn->to());
            cnn->setTo(strPort.c_str());
        }
    }

    /**
     * Some users tend to introduce YARP port dependencies inside application
     * description file. This should be avoided in future; since it is a module
     * by itself requires resources and not the application.
     * However, YARP port dependencies from application description file will
     * not be interpreted as true resource's dependencies and has no effect on
     * decision making of the algorithm.
     */
    if(application->resourcesCount())
    {
        MultiResource mres;
        ostringstream strLabel;
        strLabel<<application->getLabel()<<":MultipleResource";
        mres.setLabel(strLabel.str().c_str());
        mres.setName("MultipleResource");
        mres.setOwner(application);
        for(int i=0; i<application->resourcesCount(); i++)
             mres.addResource(application->getResourceAt(i));
        Node* node = tmpGraph.addNode(&mres);
        tmpGraph.addLink(application, node, 0);
    }

    return true;
}


GenericResource* KnowledgeBase::findResByName(Graph& graph, const char* szName)
{
    for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)
    {
        GenericResource* res = dynamic_cast<GenericResource*>(*itr);
        if(res)
            if(string(res->getName()) == string(szName))
                return res;
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
            if(compareString(input->getPort(), szPort))
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

bool KnowledgeBase::reasolveDependency(const char* szAppName,
                    bool bAutoDependancy, bool bSilent)
{
    ErrorLogger* logger = ErrorLogger::Instance();
    Application* app = dynamic_cast<Application*>(kbGraph.getNode(szAppName));
    if(!app)
    {
        ostringstream msg;
        msg<<"Application "<<string(szAppName)<<" not found.";
        logger->addError(msg.str().c_str());
        mainApplication = NULL;
        return false;
    }

    return reasolveDependency(app, bAutoDependancy, bSilent);
}

bool KnowledgeBase::reasolveDependency(Application* app,
                    bool bAutoDependancy, bool bSilent)
{
    ErrorLogger* logger = ErrorLogger::Instance();

    if(!kbGraph.hasNode(app))
    {
        ostringstream msg;
        msg<<"Application "<<app->getName()<<" not found.";
        logger->addError(msg.str().c_str());
        mainApplication = NULL;
        return false;
    }

    tmpGraph.clear();
    // Adding application to tmpGraph
    mainApplication = replicateApplication(tmpGraph,
                                            app,
                                            app->getLabel());

    // extend application to its child applications and modules
    if(!makeupApplication(mainApplication))
    {
        mainApplication = NULL;
        return false;
    }

    // Adding all resources which are providers to tmpGraph
    // providers are those with no owner.
    for(GraphIterator itr=kbGraph.begin(); itr!=kbGraph.end(); itr++)
        if(dynamic_cast<GenericResource*>(*itr))
        {

            if(!dynamic_cast<GenericResource*>(*itr)->owner())
                replicateResource(tmpGraph,
                                  (GenericResource*)(*itr),
                                  (*itr)->getLabel());
        }

    // make resources links
    makeResourceLinks(tmpGraph);

    ResourcePContainer resources;
    ModulePContainer modules;
    CnnContainer connections;
    tmpGraph.setSatisfied(false);
    bool ret = reason(&tmpGraph, mainApplication,
                      modules, resources, connections,
                      bAutoDependancy, bSilent);

    // removing double nodes
    selconnections.clear();
    selmodules.clear();
    selresources.clear();

    for(ResourcePIterator itr=resources.begin(); itr!=resources.end(); itr++)
    {
       MultiResource* mres = dynamic_cast<MultiResource*>(*itr);
       if(mres)
       {
           for(int i=0; i<mres->resourceCount(); i++)
           {
                ResYarpPort* yres = dynamic_cast<ResYarpPort*>(&mres->getResourceAt(i));
                if(yres && (find(selresources.begin(), selresources.end(), yres)
                    == selresources.end()))
                    selresources.push_back(yres);

                // adding ResYarpPort from modules dependencies
                /*
                Computer* comp = dynamic_cast<Computer*>(&mres->getResourceAt(i));
                if(comp)
                {
                    for(int j=0; j<comp->peripheralCount(); j++)
                    {
                        ResYarpPort* yres = dynamic_cast<ResYarpPort*>(&comp->getPeripheralAt(j));
                        if(yres && (find(selresources.begin(), selresources.end(), yres)
                            == selresources.end()))
                            selresources.push_back(yres);
                    }

                }*/
           }
       }
       else
       {
            if(find(selresources.begin(), selresources.end(), (*itr))
                == selresources.end())
                selresources.push_back((*itr));
       }
    }

    for(ModulePIterator itr=modules.begin(); itr!=modules.end(); itr++)
    {
       if(find(selmodules.begin(), selmodules.end(), (*itr))
          == selmodules.end())
            selmodules.push_back((*itr));
    }

    selconnections = connections;

    /*
    cout<<"Modules: "<<endl;
    for(ModulePIterator mod = selmodules.begin(); mod != selmodules.end(); mod++)
        cout<<(*mod)->getName()<<" on "<<(*mod)->getHost()<<endl;

    cout<<"Resources: "<<endl;
    for(ResourcePIterator res = selresources.begin(); res != selresources.end(); res++)
        cout<<(*res)->getName()<<endl;

    cout<<"Connections: "<<endl;
    for(CnnIterator con = selconnections.begin(); con != selconnections.end(); con++)
        cout<<(*con).from()<<", "<<(*con).to()<<endl;
    */

    /*
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


    // updating extra node links
    updateExtraLink(tmpGraph, &selconnections);

    tmpGraph.setSatisfied(false);
    if(!bestDependancyPath(application, &selnodes, bAutoDependancy))
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
    */
    return ret;
}


bool KnowledgeBase::updateApplication(Graph& graph, Application* app,
                            ApplicationInterface* iapp )
{
    __CHECK_NULLPTR(app);
    __CHECK_NULLPTR(iapp);

    if(strlen(iapp->getPrefix()))
        app->setPrefix(iapp->getPrefix());
    return true;
}

bool KnowledgeBase::updateModule(Graph& graph,
                    Module* module, ModuleInterface* imod )
{
    __CHECK_NULLPTR(module);
    __CHECK_NULLPTR(imod);

    if(strlen(imod->getHost()))
    {
        module->setHost(imod->getHost());
        module->setForced(true);
    }

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
            InputData* input = dynamic_cast<InputData*>(module->getLinkAt(i).to());
            if(input)
            {
                string strPort = string(imod->getPrefix()) + string(input->getPort());
                input->setPort(strPort.c_str());
            }
        }

        for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)
            if((*itr)->getType() == OUTPUTD)
            {
                OutputData* output = (OutputData*)(*itr);
                Module* producer = dynamic_cast<Module*>(output->getLinkAt(0).to());
                if(producer == module)
                {
                    string strPort = string(imod->getPrefix())
                                     + string(output->getPort());
                    output->setPort(strPort.c_str());
                }
            }
    }

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
GenericResource* KnowledgeBase::replicateResource(Graph& graph,
                            GenericResource* res, const char* szLabel)
{
    __CHECK_NULLPTR(res);
    GenericResource* newres = (GenericResource*) res->clone();
    newres->setLabel(szLabel);
    newres->removeAllSuc();
    /*Adding new resource to the graph */
    GenericResource* resource = (GenericResource*)graph.addNode(newres);
    delete newres;
    return resource;
}


Module* KnowledgeBase::addModuleToGraph(Graph& graph, Module* module)
{
    ErrorLogger* logger  = ErrorLogger::Instance();

    if(!moduleCompleteness(module))
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

    /* Adding resources to the graph
     * all multiple resources will be add as single MultiResource entity
     */
    MultiResource mres;
    ostringstream strLabel;
    strLabel<<module->getLabel()<<":MultipleResource";
    mres.setLabel(strLabel.str().c_str());
    mres.setName("MultipleResource");
    mres.setOwner(module);
    for(int i=0; i<module->resourceCount(); i++)
        mres.addResource(module->getResourceAt(i));
    Node* node = graph.addNode(&mres);
    graph.addLink(module, node, 0);

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
        const char* szType = module->getInputAt(i).getName();
        const char* szPort = module->getInputAt(i).getPort();
        if(!strlen(szType))
            logger->addWarning(string(module->getName()) + string(" has an input with no type."));
        if(!strlen(szPort))
            logger->addWarning(string(module->getName()) + string(" has an input with no port."));
    }

    /* Checking outputs name and port */
    for(int i=0; i<module->outputCount(); i++)
    {
        const char* szType = module->getOutputAt(i).getName();
        const char* szPort = module->getOutputAt(i).getPort();
        if(!strlen(szType))
            logger->addWarning(string(module->getName()) + string(" has an output with no type."));
        if(!strlen(szPort))
            logger->addWarning(string(module->getName()) + string(" has an output with no port."));
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

        GenericResource* res = findResByName(graph, cnn.from());
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

void KnowledgeBase::updateResourceWeight(Graph& graph,
                                        GenericResource* resource, float weight)
{
    for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)
    {
        for(int i=0; i<(*itr)->sucCount(); i++)
            if((*itr)->getLinkAt(i).to() == resource)
                (*itr)->getLinkAt(i).setWeight(weight);
    }

}

void KnowledgeBase::makeResourceLinks(Graph& graph)
{
    /**
     * link inputs to relevant outputs and resources
     */
    for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)
    {
        GenericResource* resource = dynamic_cast<GenericResource*>(*itr);
        if(resource && resource->owner())
        {
            resource->removeAllSuc();
            Module* module = dynamic_cast<Module*>(resource->owner());
            if(module && module->getForced())
            {
                // we should create a provider resource with host name and
                // connect it to resource
                GenericResource* provider = findResByName(graph, module->getHost());
                if(!provider)
                {
                    Computer comp;
                    comp.setLabel(module->getHost());
                    comp.setName(module->getHost());
                    provider = (GenericResource*) graph.addNode(&comp);
                }
                float w = 0.0;
                if(dynamic_cast<Computer*>(provider))
                    w = calculateLoad((Computer*)provider);
                graph.addLink(resource, provider, w, false);
            }
            else if((module && !module->getForced()))
            {
                // linking resource providers to the relevant resources
                for(GraphIterator itr2=graph.begin(); itr2!=graph.end(); itr2++)
                {
                    GenericResource* provider = dynamic_cast<GenericResource*>(*itr2);
                    if(provider && !provider->owner())
                        if(provider->satisfy(resource))
                        {
                            float w = 0.0;
                            if(dynamic_cast<Computer*>(provider))
                                w = calculateLoad((Computer*)provider);
                            graph.addLink(resource, provider, w, false);
                        }
                }
            }
        }
    }
}


float KnowledgeBase::calculateLoad(Computer* comp)
{
    LoadAvg load = comp->getProcessor().getCPULoad();
    int siblings = comp->getProcessor().getSiblings();
    if(siblings == 0) siblings = 1;
    //cout<<comp->getName()<<": "<<load.loadAverage1<<", "<<load.loadAverage5<<", "<<load.loadAverage15;
    //cout<<" ("<<siblings<<")"<<endl;
    float lavg = (float)((load.loadAverage1*15.0 +
                          load.loadAverage5*10.0 +
                          load.loadAverage15*1.0) / 26.0);
    return (lavg/(float)siblings);
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

bool KnowledgeBase::constrainSatisfied(Node* node,
                                       bool bAutoDependancy,
                                       bool bSilent)
{
    ErrorLogger* logger  = ErrorLogger::Instance();

    // if it's a dependency resource with no provider
    GenericResource* resource = dynamic_cast<GenericResource*>(node);
    if(resource && resource->isLeaf() &&
       dynamic_cast<Module*>(resource->owner()))
    {
        if(!bSilent)
        {
            ostringstream msg;
            msg<<"Some resource dependencies of ";
            msg<<dynamic_cast<Module*>(resource->owner())->getName();
            msg<<" are not satisfied.";
            logger->addWarning(msg);
        }
        return false;
    }

    return true;
}

/*
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
*/


bool KnowledgeBase::reason(Graph* graph, Node* initial,
                                 ModulePContainer &modules,
                                 ResourcePContainer& resources,
                                 CnnContainer &connections,
                                 bool bAutoDependancy, bool bSilent)
{
    if(!constrainSatisfied(initial, bAutoDependancy, bSilent))
    {
        /* if it's a resource dependency */
        GenericResource* resource = dynamic_cast<GenericResource*>(initial);
        if(resource && resource->owner())
            resources.push_back(resource);
        return false;
    }

    if(initial->isLeaf())
    {
        // adding resource
        if(dynamic_cast<GenericResource*>(initial))
            resources.push_back(dynamic_cast<GenericResource*>(initial));

        // adding connections
        Application* application = dynamic_cast<Application*>(initial);
        if(application)
            for(int i=0; i<application->connectionCount(); i++)
                connections.push_back(application->getConnectionAt(i));

        initial->setSatisfied(true);
        initial->setVisited(false);
        return true;
    }

    /* now we are visiting this node */
    initial->setVisited(true);
    bool bPathFound = false;

    Link* candidateLink = NULL;
    float weight = -1.0;

    for(int i=0; i<initial->sucCount(); i++)
    {
        Link l = initial->getLinkAt(i);
        Node* current  = l.to();

        /* we are not interested in virtual links
         * and we are not meeting a visited node to avoid looping
         */
        if(!l.isVirtual() && !current->isVisited())
        {
            ModulePContainer subModules;
            ResourcePContainer subResources;
            CnnContainer subConnections;
            bool ret = reason(graph, current,
                              subModules, subResources, subConnections,
                              bAutoDependancy, bSilent);
            if(ret)
                bPathFound = true;
            else if(dynamic_cast<Application*>(initial) ||
                    dynamic_cast<Module*>(initial))
                    bPathFound = false;

            /* we meet a conjunctive node and we need to copy all the
             *  selected successors
             */
            if (dynamic_cast<Application*>(initial) ||
                dynamic_cast<Module*>(initial))
            {
                modules.insert(modules.end(), subModules.begin(), subModules.end());
                resources.insert(resources.end(), subResources.begin(), subResources.end());
                connections.insert(connections.end(), subConnections.begin(), subConnections.end());
            }

            /* A disjunctive node; we should choose the best path */
            else if((weight<0.0) || (l.weight() < weight))
            {
                weight = l.weight();
                candidateLink = &initial->getLinkAt(i);
                modules = subModules;
                resources = subResources;
                connections = subConnections;
            }

        }
    } // end for

    /* if it's a resource dependency with a solution */
    GenericResource* resource = dynamic_cast<GenericResource*>(initial);
    if(resource && resource->owner() && candidateLink)
    {
        Module* module = dynamic_cast<Module*>(resource->owner());
        GenericResource* provider = dynamic_cast<GenericResource*>(candidateLink->to());
        if(module && provider)
        {
            // setting module's host property
            module->setHost(provider->getName());

            // we need to update the weight of all links to the selected provider
            // with load balancer tunning value;
            // It will help for better load distribution among nodes
            Computer* comp = dynamic_cast<Computer*>(provider);
            float default_tunning = 0.1F;
            if(comp && (comp->getProcessor().getSiblings() > 0))
                default_tunning = 1.0F / (float)comp->getProcessor().getSiblings();
            float tunner = (module->getRank()<10)? default_tunning : (float)module->getRank()/100.0F;
            updateResourceWeight(*graph, provider, candidateLink->weight()+tunner);
        }
        resources.push_back(resource);
    }

    /* adding current module to the modules list.*/
    if(dynamic_cast<Module*>(initial))
        modules.push_back(dynamic_cast<Module*>(initial));

    /* we should add all the connections */
    Application* application = dynamic_cast<Application*>(initial);
    if(application)
        for(int i=0; i<application->connectionCount(); i++)
            connections.push_back(application->getConnectionAt(i));

    initial->setSatisfied(bPathFound);
    initial->setVisited(false);
    return bPathFound;
}


const char* KnowledgeBase::createDataLabel(const char* modlabel,
                                           const char* port,
                                           const char* postfix)
{
    static string name;
    name.clear();
    name = string(modlabel) + string(port);
    if(postfix)
        name += string(postfix);
    return name.c_str();
}




