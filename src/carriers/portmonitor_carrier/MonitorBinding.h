// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MONITORBINDING_INC_
#define _MONITORBINDING_INC_

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Property.h>


class MonitorBinding 
{

public:
    virtual ~MonitorBinding();   
    virtual bool loadScript(const char* filename) = 0;
    virtual bool setParams(const yarp::os::Property& params) = 0;
    virtual bool getParams(yarp::os::Property& params) = 0;
    virtual yarp::os::ConnectionReader& updateData(yarp::os::ConnectionReader& reader) = 0;
   
    /**
     * factory method 
     */
    static MonitorBinding *create(const char* script_type);

};

#endif //_MONITORBINDING_INC_


