// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MONITOR_SHAREDLIB_INC_
#define _MONITOR_SHAREDLIB_INC_

#include <string>
#include <yarp/os/ConstString.h>
#include <yarp/os/SharedLibraryClass.h>
#include <yarp/os/SharedLibrary.h>

#include <yarp/os/MonitorObject.h>
#include "MonitorBinding.h"

class MonitorSharedLib : public MonitorBinding
{

public:
    MonitorSharedLib(void);
    virtual ~MonitorSharedLib();
    
    bool load(const yarp::os::Property &options);
    bool setParams(const yarp::os::Property& params);
    bool getParams(yarp::os::Property& params);

    bool acceptData(yarp::os::Things &thing);
    yarp::os::Things& updateData(yarp::os::Things &thing);

    bool peerTrigged(void);
    bool canAccept(void);

    bool setAcceptConstraint(const char* constraint) {
        if(!constraint)
            return false;        
        MonitorSharedLib::constraint = constraint;
        return true;
    }

    const char* getAcceptConstraint(void) {
        return constraint.c_str();
    }

    bool hasAccept() { return true; }
    bool hasUpdate() { return true; } 

private:
    std::string constraint;
    yarp::os::SharedLibraryClassFactory<yarp::os::MonitorObject> monitorFactory;
    yarp::os::SharedLibraryClass<yarp::os::MonitorObject> monitor;
};

#endif //_MONITOR_SHAREDLIB_INC_


