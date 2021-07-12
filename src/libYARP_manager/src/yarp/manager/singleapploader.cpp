/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
