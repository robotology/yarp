/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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


SingleAppLoader::~SingleAppLoader() = default;


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
