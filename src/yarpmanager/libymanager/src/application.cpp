/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "application.h"

/**
 * Class  ModuleInterface
 */

ModuleInterface::ModuleInterface(Module* module)
{
    if(!module)
        return;

    strName = module->strName;
    strHost = module->strHost;
    strParam = module->strParam;
    strWorkDir = module->strWorkDir;
    strStdio = module->strStdio;
    strBroker = module->strBroker;
    strPrefix = module->strPrefix;
    iRank = module->iRank;
    strTag = module->getLabel();
    if(module->getModel())
        modelBase = *module->getModel();
    else
        modelBase = module->getModelBase();

   //TODO: resources should be added too
}

bool ModuleInterface::addPortmap(Portmap &portmap)
{
    portmaps.push_back(portmap);    
    return true;
}

    
bool ModuleInterface::removePortmap(Portmap& portmap)
{
    PortmapIterator itr = findPortmap(portmap);
    if(itr == portmaps.end()) 
        return true;
    portmaps.erase(itr);
    return true;
}


PortmapIterator ModuleInterface::findPortmap(Portmap& portmap) 
{
    PortmapIterator itr;
    for(itr=portmaps.begin(); itr<portmaps.end(); itr++) 
        if ((*itr) == portmap)
            return itr;
    return portmaps.end();
}


/**
 * Class Application
 */
Application::Application(void) : Node(APPLICATION) 
{
    clear();
}


Application::Application(const char* szName) : Node(APPLICATION) 
{
    clear();
    setName(szName);
}


Application::Application(const Application &app) : Node(app)
{
    strName = app.strName;
    strVersion = app.strVersion;
    strDescription = app.strDescription;
    authors = app.authors;
    Imodules = app.Imodules;
    connections = app.connections;
    arbitrators = app.arbitrators;
    Iapplications = app.Iapplications;
    resources = app.resources;
    strXmlFile = app.strXmlFile;
    strPrefix = app.strPrefix;
    strBasePrefix = app.strBasePrefix;
}


Application::~Application() { }


Node* Application::clone(void)
{
    Application* app = new Application(*this);
    return app; 
}


bool Application::addImodule(ModuleInterface &imod)
{
    Imodules.push_back(imod);
    return true;
}


bool Application::removeImodule(ModuleInterface& imod)
{
    IModuleIterator itr = findImodule(imod);
    if(itr == Imodules.end()) 
        return true;
    Imodules.erase(itr);
    return true;    
}


Connection& Application::addConnection(Connection &cnn)
{
    connections.push_back(cnn);
    CnnIterator itr = findConnection(cnn);
    return (*itr);
}


bool Application::removeConnection(Connection& cnn)
{
    CnnIterator itr = findConnection(cnn);
    if(itr == connections.end()) 
        return true;
    connections.erase(itr);
    return true;    
    
}

Arbitrator& Application::addArbitrator(Arbitrator &arb)
{
    arbitrators.push_back(arb);
    ArbIterator itr = findArbitrator(arb);
    return(*itr);
}

bool Application::removeArbitrator(Arbitrator& arb)
{
    ArbIterator itr = findArbitrator(arb);
    if(itr == arbitrators.end()) 
        return true;
    arbitrators.erase(itr);
    return true;     
}


/*
void Application::updateConnectionPrefix(void)
{
    CnnIterator itr;
    for(itr=connections.begin(); itr<connections.end(); itr++) 
    {
        string strPort; 
        if(!((*itr).isExternalFrom()))
        {
            strPort = strPrefix + string((*itr).from()); 
            (*itr).setFrom(strPort.c_str());
        }

        if(!((*itr).isExternalTo()))
        {
            strPort = strPrefix + string((*itr).to()); 
            (*itr).setTo(strPort.c_str());
        }
    }
}
*/

bool Application::addIapplication(ApplicationInterface &iapp)
{
    Iapplications.push_back(iapp);
    return true;
}


bool Application::removeIapplication(ApplicationInterface& iapp)
{
    IApplicationIterator itr = findIapplication(iapp);
    if(itr == Iapplications.end()) 
        return true;
    Iapplications.erase(itr);
    return true;
}

bool Application::addResource(ResYarpPort &res)
{
    resources.push_back(res);
    return true;
}


bool Application::removeResource(ResYarpPort& res)
{
    ResourceIterator itr = findResource(res);
    if(itr == resources.end()) 
        return true;
    resources.erase(itr);
    return true;
}

bool Application::removeAuthor(Author& author)
{
    AuthorIterator itr;
    for(itr=authors.begin(); itr<authors.end(); itr++)
        if((*itr) == author)
        {
            authors.erase(itr);
            return true;
        }    
    return true;        
}


IModuleIterator Application::findImodule(ModuleInterface& imod) 
{
    IModuleIterator itr;
    for(itr=Imodules.begin(); itr<Imodules.end(); itr++) 
        if ((*itr) == imod)
            return itr;
    return Imodules.end();
}


CnnIterator Application::findConnection(Connection& cnn)
{
    CnnIterator itr;
    for(itr=connections.begin(); itr<connections.end(); itr++) 
        if ((*itr) == cnn)
            return itr;
    return connections.end();   
}

ArbIterator Application::findArbitrator(Arbitrator& arb)
{
    ArbIterator itr;
    for(itr=arbitrators.begin(); itr<arbitrators.end(); itr++) 
        if ((*itr) == arb)
            return itr;
    return arbitrators.end();   
}


IApplicationIterator Application::findIapplication(ApplicationInterface& iapp) 
{
    IApplicationIterator itr;
    for(itr=Iapplications.begin(); itr<Iapplications.end(); itr++) 
        if ((*itr) == iapp)
            return itr;
    return Iapplications.end();
}

ResourceIterator Application::findResource(ResYarpPort& res) 
{
    ResourceIterator itr;
    for(itr=resources.begin(); itr<resources.end(); itr++) 
        if ((*itr) == res)
            return itr;
    return resources.end();
}



