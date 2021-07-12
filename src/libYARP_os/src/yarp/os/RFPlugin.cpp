/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/RFPlugin.h>

#include <yarp/os/RFModule.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/os/YarpPluginSelector.h>
#include <yarp/os/impl/RFModuleFactory.h>

using namespace std;
using namespace yarp::os;

class RFModuleSelector :
        public YarpPluginSelector
{
    bool select(Searchable& options) override
    {
        return options.check("type", Value("none")).asString() == "RFModule";
    }
};

struct SharedRFPlugin
{
    YarpPlugin<RFModule> yarpPlugin;
    SharedLibraryClass<RFModule> sharedLibClass;
    RFModuleSelector selector;
};


struct RFPlugin::Private
{
    Private() = default;

    string alias;
    string name;
    string command;
    int threadID{0};
    SharedRFPlugin* shared{nullptr};

    RFModule* module{nullptr};
    ~Private()
    {
        delete shared;
    }
};

RFPlugin::RFPlugin() :
        mPriv(new Private)
{
}

RFPlugin::~RFPlugin()
{
    delete mPriv;
}

string RFPlugin::getCmd()
{
    return mPriv->command;
}

int RFPlugin::getThreadKey()
{
    return mPriv->module->getThreadKey();
}

void RFPlugin::close()
{
    mPriv->module->stopModule();
}

bool RFPlugin::isRunning()
{
    return !mPriv->module->isStopping();
}


std::pair<int, char**> str2ArgcArgv(char* str)
{
    enum
    {
        kMaxArgs = 64
    };
    int argc = 0;
    char* argv[kMaxArgs];

    char* p2 = strtok(str, " ");
    while ((p2 != nullptr) && argc < kMaxArgs - 1) {
        argv[argc++] = p2;
        p2 = strtok(nullptr, " ");
    }
    argv[argc] = nullptr;
    return make_pair(argc, argv);
}

bool RFPlugin::open(const string& inCommand)
{
    ResourceFinder rf;
    string name = inCommand.substr(0, inCommand.find(' '));

    char* str = new char[inCommand.size() + 1];
    memcpy(str, inCommand.c_str(), inCommand.size());
    str[inCommand.size()] = '\0';

    mPriv->command = inCommand;
    auto argcv = str2ArgcArgv(str);
    rf.configure(argcv.first, argcv.second);
    delete[] str;

    RFModule* staticmodule{nullptr};
    staticmodule = RFModuleFactory::GetInstance().GetModule(name);
    if (staticmodule != nullptr) {
        try {
            if (!staticmodule->configure(rf)) {
                return false;
            }
            staticmodule->runModuleThreaded();
            mPriv->module = staticmodule;
            return true;
        } catch (...) {
            return false;
        }
    }

    YarpPluginSettings settings;
    mPriv->shared = new SharedRFPlugin;
    mPriv->name = name;
    mPriv->shared->selector.scan();

    settings.setPluginName(mPriv->name);

    if (!settings.setSelector(mPriv->shared->selector)) {
        return false;
    }
    if (!mPriv->shared->yarpPlugin.open(settings)) {
        return false;
    }

    mPriv->shared->sharedLibClass.open(*mPriv->shared->yarpPlugin.getFactory());

    if (!mPriv->shared->sharedLibClass.isValid()) {
        return false;
    }

    settings.setLibraryMethodName(mPriv->shared->yarpPlugin.getFactory()->getName(), settings.getMethodName());
    settings.setClassInfo(mPriv->shared->yarpPlugin.getFactory()->getClassName(), mPriv->shared->yarpPlugin.getFactory()->getBaseClassName());

    bool ret{false};
    try {
        ret = mPriv->shared->sharedLibClass.getContent().configure(rf);
    } catch (...) {
        return false;
    }

    if (ret) {
        mPriv->shared->sharedLibClass->runModuleThreaded();
    }
    mPriv->module = &(mPriv->shared->sharedLibClass.getContent());
    return ret;
}
