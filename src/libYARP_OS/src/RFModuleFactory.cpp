/*
* Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
* All rights reserved.
*
* This software may be modified and distributed under the terms of the
* BSD-3-Clause license. See the accompanying LICENSE file for details.
*/
#include <yarp/os/YarpPluginSelector.h>
#include <yarp/os/YarpPluginSettings.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/os/RFModuleFactory.h>
#include <map>
using namespace std;
using namespace yarp::os;

class RFModuleSelector : public YarpPluginSelector
{
    virtual bool select(Searchable& options) override
    {
        return options.check("type",Value("none")).asString() == "RFModule";
    }
};

struct SharedRFPlugin
{
    YarpPlugin<RFModule>         yarpPlugin;
    SharedLibraryClass<RFModule> sharedLibClass;
    RFModuleSelector             selector;
};

struct RFPlugin::RFPlugin_Private
{
    RFPlugin_Private() = default;

    string          alias;
    string          name;
    string          command;
    int             threadID{0};
    SharedRFPlugin* shared{nullptr};
    
    RFModule*       module{nullptr};
    ~RFPlugin_Private()
    {
        delete shared;
    }
};

RFPlugin::RFPlugin() :
    impl(new RFPlugin_Private)
{
}

RFPlugin::~RFPlugin()
{
    delete impl;
}

string RFPlugin::getCmd()
{
    return impl->command;
}

string RFPlugin::getAlias()
{
    return impl->alias;
}

int RFPlugin::getThreadKey()
{
    return impl->module->getThreadKey();
}

void RFPlugin::close()
{
    impl->module->stopModule();
}

bool RFPlugin::isRunning()
{
    return !impl->module->isStopping();
}

bool RFPlugin::open(const string& inCommand)
{
    ResourceFinder     rf;
    string name = inCommand.substr(0, inCommand.find(" "));

    char* str = new char[inCommand.size() + 1];
    memcpy(str, inCommand.c_str(), inCommand.size());
    str[inCommand.size()] = '\0';

    enum { kMaxArgs = 64 };
    int argc = 0;
    char* argv[kMaxArgs];

    char* p2 = strtok(str, " ");
    while (p2 && argc < kMaxArgs - 1)
    {
        argv[argc++] = p2;
        p2 = strtok(0, " ");
    }
    argv[argc] = 0;

    impl->command = inCommand;
    rf.configure(argc, argv);
    delete[] str;

    RFModule* staticmodule{nullptr};
    staticmodule = RFModuleFactory::GetInstance().GetModule(name);
    if (staticmodule)
    {
        try
        {
            if(!staticmodule->configure(rf)) return false;
            staticmodule->runModuleThreaded();
            impl->module = staticmodule;
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    YarpPluginSettings settings;
    impl->shared = new SharedRFPlugin;
    impl->name = name;
    impl->shared->selector.scan();

    settings.setPluginName(impl->name);
    settings.setVerboseMode(true);

    if (!settings.setSelector(impl->shared->selector)) return false;
    if (!impl->shared->yarpPlugin.open(settings)) return false;
    
    impl->shared->sharedLibClass.open(*impl->shared->yarpPlugin.getFactory());
    
    if (!impl->shared->sharedLibClass.isValid()) return false;

    settings.setLibraryMethodName(impl->shared->yarpPlugin.getFactory()->getName(), settings.getMethodName());
    settings.setClassInfo(impl->shared->yarpPlugin.getFactory()->getClassName(), impl->shared->yarpPlugin.getFactory()->getBaseClassName());

    bool ret{ false };
    try
    {
        ret = impl->shared->sharedLibClass.getContent().configure(rf);
    }
    catch (...)
    {
        return false;
    }

    if(ret) impl->shared->sharedLibClass->runModuleThreaded();
    impl->module = &(impl->shared->sharedLibClass.getContent());
    return ret;
}

struct RFModuleFactory::Private
{
    map<string, RFModule*(*)(void)> delegates;
};

RFModuleFactory::RFModuleFactory() : impl(new Private)
{
    //add rfmodule here
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

