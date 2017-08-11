/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Istituto Italiano di Tecnologia (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/singleapploader.h>
#include <yarp/manager/utility.h>


using namespace std;
using namespace yarp::manager;


SingleAppLoader::SingleAppLoader(const char* szModule, const char* szHost)
{
    if(szModule) strModule = szModule;
    if(szHost) strHost = szHost;
}


SingleAppLoader::~SingleAppLoader()
{
}


bool SingleAppLoader::init()
{
    ErrorLogger* logger  = ErrorLogger::Instance();
    app.clear();
    if(strModule.empty())
    {
        logger->addError("Empty module name.");
        return false;
    }

    app.setName(strModule.c_str());
    ModuleInterface module(strModule.c_str());
    module.setHost(strHost.c_str());
    app.addImodule(module);
    return true;
}


void SingleAppLoader::fini()
{
    app.clear();
}


Application* SingleAppLoader::getNextApplication()
{
    return &app;
}
