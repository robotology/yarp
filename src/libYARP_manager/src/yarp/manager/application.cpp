/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/application.h>

using namespace yarp::manager;

/**
 * Class  ModuleInterface
 */
ModuleInterface::ModuleInterface(Module* module) :
    iRank(0),
    waitStart(0.0),
    waitStop(0.0)
{
    if (!module) {
        return;
    }

    strName = module->strName;
    strHost = module->strHost;
    strParam = module->strParam;
    strWorkDir = module->strWorkDir;
    strStdio = module->strStdio;
    strBroker = module->strBroker;
    strPrefix = module->strPrefix;
    strEnvironment = module->strEnvironment;
    iRank = module->iRank;
    strTag = module->getLabel();
    strDisplay = module->getDisplay();
    waitStart = module->getPostExecWait();
    waitStop = module->getPostStopWait();
    if (module->getModel()) {
        modelBase = *module->getModel();
    } else {
        modelBase = module->getModelBase();
    }

   //TODO: resources should be added too
}

bool ModuleInterface::addPortmap(Portmap &portmap)
{
    portmaps.push_back(portmap);
    return true;
}


bool ModuleInterface::removePortmap(Portmap& portmap)
{
    auto itr = findPortmap(portmap);
    if (itr == portmaps.end()) {
        return true;
    }
    portmaps.erase(itr);
    return true;
}


PortmapIterator ModuleInterface::findPortmap(Portmap& portmap)
{
    PortmapIterator itr;
    for (itr = portmaps.begin(); itr < portmaps.end(); itr++) {
        if ((*itr) == portmap) {
            return itr;
        }
    }
    return portmaps.end();
}


/**
 * Class Application
 */
Application::Application() : Node(APPLICATION)
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
    appOwner = app.appOwner;
}


Application::~Application() = default;


Node* Application::clone()
{
    auto* app = new Application(*this);
    return app;
}


bool Application::addImodule(ModuleInterface &imod)
{
    Imodules.push_back(imod);
    return true;
}


bool Application::removeImodule(ModuleInterface& imod)
{
    auto itr = findImodule(imod);
    if (itr == Imodules.end()) {
        return true;
    }
    Imodules.erase(itr);
    return true;
}


Connection& Application::addConnection(Connection &cnn)
{
    connections.push_back(cnn);
    auto itr = findConnection(cnn);
    return (*itr);
}


bool Application::removeConnection(Connection& cnn)
{
    auto itr = findConnection(cnn);
    if (itr == connections.end()) {
        return true;
    }
    connections.erase(itr);
    return true;

}

Arbitrator& Application::addArbitrator(Arbitrator &arb)
{
    arbitrators.push_back(arb);
    auto itr = findArbitrator(arb);
    return(*itr);
}

bool Application::removeArbitrator(Arbitrator& arb)
{
    auto itr = findArbitrator(arb);
    if (itr == arbitrators.end()) {
        return true;
    }
    arbitrators.erase(itr);
    return true;
}


/*
void Application::updateConnectionPrefix()
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
    auto itr = findIapplication(iapp);
    if (itr == Iapplications.end()) {
        return true;
    }
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
    auto itr = findResource(res);
    if (itr == resources.end()) {
        return true;
    }
    resources.erase(itr);
    return true;
}

bool Application::removeAuthor(Author& author)
{
    AuthorIterator itr;
    for (itr = authors.begin(); itr < authors.end(); itr++) {
        if((*itr) == author)
        {
            authors.erase(itr);
            return true;
        }
    }
    return true;
}


IModuleIterator Application::findImodule(ModuleInterface& imod)
{
    IModuleIterator itr;
    for (itr = Imodules.begin(); itr < Imodules.end(); itr++) {
        if ((*itr) == imod) {
            return itr;
        }
    }
    return Imodules.end();
}


CnnIterator Application::findConnection(Connection& cnn)
{
    CnnIterator itr;
    for (itr = connections.begin(); itr < connections.end(); itr++) {
        if ((*itr) == cnn) {
            return itr;
        }
    }
    return connections.end();
}

ArbIterator Application::findArbitrator(Arbitrator& arb)
{
    ArbIterator itr;
    for (itr = arbitrators.begin(); itr < arbitrators.end(); itr++) {
        if ((*itr) == arb) {
            return itr;
        }
    }
    return arbitrators.end();
}


IApplicationIterator Application::findIapplication(ApplicationInterface& iapp)
{
    IApplicationIterator itr;
    for (itr = Iapplications.begin(); itr < Iapplications.end(); itr++) {
        if ((*itr) == iapp) {
            return itr;
        }
    }
    return Iapplications.end();
}

ResourceIterator Application::findResource(ResYarpPort& res)
{
    ResourceIterator itr;
    for (itr = resources.begin(); itr < resources.end(); itr++) {
        if ((*itr) == res) {
            return itr;
        }
    }
    return resources.end();
}
