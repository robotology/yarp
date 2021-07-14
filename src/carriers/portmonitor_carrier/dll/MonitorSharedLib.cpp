/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/YarpPlugin.h>

#include "MonitorSharedLib.h"


using namespace yarp::os;
using namespace std;



class MonitorSelector : public YarpPluginSelector {
    bool select(Searchable& options) override {
        return options.check("type",Value("none")).asString() == "portmonitor";
    }
};

/**
 * Class MonitorSharedLib
 */
MonitorSharedLib::MonitorSharedLib()
{
}

MonitorSharedLib::~MonitorSharedLib()
{
    if (monitor.isValid()) {
        monitor->destroy();
    }
    monitor.close();
}

bool MonitorSharedLib::load(const yarp::os::Property& options)
{
    MonitorSelector selector;
    selector.scan();

    settings.setPluginName(options.find("filename").asString());
    if (!settings.setSelector(selector)) {
        return false;
    }

    if (!plugin.open(settings)) {
        return false;
    }

    monitor.open(*plugin.getFactory());
    if (!monitor.isValid()) {
        return false;
    }

    settings.setLibraryMethodName(plugin.getFactory()->getName(),
                                  settings.getMethodName());
    settings.setClassInfo(plugin.getFactory()->getClassName(),
                          plugin.getFactory()->getBaseClassName());

    return monitor->create(options);
}

bool MonitorSharedLib::setParams(const Property &params)
{
    return monitor->setparam(params);
}

bool MonitorSharedLib::getParams(yarp::os::Property& params)
{
    return monitor->getparam(params);
}

bool MonitorSharedLib::acceptData(yarp::os::Things& thing)
{
    return monitor->accept(thing);
}


yarp::os::Things& MonitorSharedLib::updateData(Things &thing)
{
    return monitor->update(thing);
}

yarp::os::Things& MonitorSharedLib::updateReply(Things &thing)
{
    return monitor->updateReply(thing);
}


bool MonitorSharedLib::peerTrigged()
{
    monitor->trig();
    return true;
}

bool MonitorSharedLib::canAccept()
{
    if (constraint == "") {
        return true;
    }
     //TODO: constraint checking should be implemented here!
     return true;
}
