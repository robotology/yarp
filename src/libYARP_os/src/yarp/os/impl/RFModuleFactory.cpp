/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/RFModuleFactory.h>

#include <yarp/os/YarpPluginSettings.h>

#include <map>

using namespace yarp::os;
using namespace yarp::os::impl;

class RFModuleFactory::Private
{
public:
    std::map<std::string, RFModule* (*)(void)> delegates;
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

void RFModuleFactory::AddModule(const std::string& name, RFModule* (*module)())
{
    GetInstance().mPriv->delegates[name] = module;
}

RFModule* RFModuleFactory::GetModule(const std::string& name)
{
    if (mPriv->delegates.find(name) != mPriv->delegates.end()) {
        return mPriv->delegates[name]();
    }

    return nullptr;
}
