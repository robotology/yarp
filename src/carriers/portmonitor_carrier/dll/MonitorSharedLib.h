// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
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

#include "MonitorObject.h"
#include "MonitorBinding.h"

class MonitorSharedLib : public MonitorBinding
{

public:
    MonitorSharedLib(void);
    virtual ~MonitorSharedLib();
    
    bool load(const char* script_file);
    bool setParams(const yarp::os::Property& params);
    bool getParams(yarp::os::Property& params);

    bool acceptData(yarp::os::Things& thing);
    yarp::os::Things& updateData(yarp::os::Things& thing);

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

    bool hasAccept();
    bool hasUpdate(); 

private:
    std::string constraint;
    yarp::os::SharedLibraryClassFactory<MonitorObject> monitorFactory;
    yarp::os::SharedLibraryClass<MonitorObject> monitor;
};

#endif //_MONITOR_SHAREDLIB_INC_


