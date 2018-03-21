/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef MONITOR_SHAREDLIB_INC
#define MONITOR_SHAREDLIB_INC

#include <string>
#include <yarp/os/ConstString.h>
#include <yarp/os/SharedLibraryClass.h>
#include <yarp/os/SharedLibrary.h>
#include <yarp/os/YarpPlugin.h>

#include <yarp/os/MonitorObject.h>
#include "MonitorBinding.h"

class MonitorSharedLib : public MonitorBinding
{

public:
    MonitorSharedLib(void);
    virtual ~MonitorSharedLib();

    bool load(const yarp::os::Property &options) override;
    bool setParams(const yarp::os::Property& params) override;
    bool getParams(yarp::os::Property& params) override;

    bool acceptData(yarp::os::Things &thing) override;
    yarp::os::Things& updateData(yarp::os::Things &thing) override;
    yarp::os::Things& updateReply(yarp::os::Things &thing) override;

    bool peerTrigged(void) override;
    bool canAccept(void) override;

    bool setAcceptConstraint(const char* constraint) override {
        if(!constraint)
            return false;
        MonitorSharedLib::constraint = constraint;
        return true;
    }

    const char* getAcceptConstraint(void) override {
        return constraint.c_str();
    }

    bool hasAccept() override { return true; }
    bool hasUpdate() override { return true; }
    bool hasUpdateReply() override { return true; }

private:
    std::string constraint;
    yarp::os::YarpPluginSettings settings;
    yarp::os::YarpPlugin<yarp::os::MonitorObject> plugin;
    yarp::os::SharedLibraryClass<yarp::os::MonitorObject> monitor;
};

#endif //_MONITOR_SHAREDLIB_INC_


