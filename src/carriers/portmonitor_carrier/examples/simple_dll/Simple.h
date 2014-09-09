// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef SIMPLE_INC
#define SIMPLE_INC

#include "AbstractMonitorObject.h"

class AbstractMonitorObjectImpl : public AbstractMonitorObject 
{
public:
    bool setParams(const yarp::os::Property& params);
    bool getParams(yarp::os::Property& params);
    
    bool hasAccept();
    bool acceptData(yarp::os::Things& thing);

    bool hasUpdate();
    yarp::os::Things& updateData(yarp::os::Things& thing);

    bool peerTrigged(void);
};

#endif
