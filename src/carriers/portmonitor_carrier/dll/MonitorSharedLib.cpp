/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/YarpPlugin.h>

#include "MonitorSharedLib.h"


using namespace yarp::os;
using namespace std;



class MonitorSelector : public YarpPluginSelector {
    virtual bool select(Searchable& options) {
        return options.check("type",Value("none")).asString() == "portmonitor";
    }
};

/**
 * Class MonitorSharedLib
 */
MonitorSharedLib::MonitorSharedLib(void)
{
    settings.setVerboseMode(true);
}

MonitorSharedLib::~MonitorSharedLib()
{
    if(monitor.isValid())
        monitor->destroy();
    monitor.close();
}

bool MonitorSharedLib::load(const yarp::os::Property& options)
{
    MonitorSelector selector;
    selector.scan();

    settings.setPluginName(options.find("filename").asString());
    settings.setVerboseMode(true);
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


bool MonitorSharedLib::peerTrigged(void)
{
    monitor->trig();
    return true;
}

bool MonitorSharedLib::canAccept(void)
{
    if(constraint == "")
        return true;
     //TODO: constraint checking should be implemented here!
     return true;
}
