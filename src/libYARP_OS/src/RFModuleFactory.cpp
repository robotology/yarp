/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/YarpPluginSettings.h>

#include <yarp/os/impl/RFModuleFactory.h>
#include <map>
using namespace std;
using namespace yarp::os;

struct RFModuleFactory::Private
{
    map<string, RFModule*(*)(void)> delegates;
};

RFModuleFactory::RFModuleFactory() : impl(new Private)
{
    //add embedded rfmodule here
}

RFModuleFactory& RFModuleFactory::GetInstance()
{
    static RFModuleFactory instance;
    return instance;
}

void RFModuleFactory::AddModule(const string &name, RFModule*(*module)(void))
{
    GetInstance().impl->delegates[name] = module;
}

RFModule* RFModuleFactory::GetModule(const string name)
{
    if(impl->delegates.find(name) != impl->delegates.end())
    {
        return impl->delegates[name]();
    }

    return nullptr;
}
