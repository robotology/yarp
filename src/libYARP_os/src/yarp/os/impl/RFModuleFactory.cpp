/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/RFModuleFactory.h>

#include <yarp/os/YarpPluginSettings.h>

#include <map>

using namespace std;
using namespace yarp::os;

class RFModuleFactory::Private
{
public:
    map<string, RFModule* (*)(void)> delegates;
};

RFModuleFactory::RFModuleFactory() :
        mPriv(new Private)
{
    //add embedded rfmodule here
}

RFModuleFactory& RFModuleFactory::GetInstance()
{
    static RFModuleFactory instance;
    return instance;
}

void RFModuleFactory::AddModule(const string& name, RFModule* (*module)())
{
    GetInstance().mPriv->delegates[name] = module;
}

RFModule* RFModuleFactory::GetModule(const string& name)
{
    if (mPriv->delegates.find(name) != mPriv->delegates.end()) {
        return mPriv->delegates[name]();
    }

    return nullptr;
}
