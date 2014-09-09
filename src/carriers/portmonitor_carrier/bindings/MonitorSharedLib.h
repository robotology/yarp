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
        //trimString(MonitorSharedLib::constraint);
        return true;
    }

    const char* getAcceptConstraint(void) {
        return constraint.c_str();
    }

    bool hasAccept() {
        return bHasAcceptCallback; 
    }

    bool hasUpdate() {
        return bHasUpdateCallback;
    }

private:
    std::string constraint;
    bool bHasAcceptCallback;
    bool bHasUpdateCallback;
};

#endif //_MONITOR_SHAREDLIB_INC_


