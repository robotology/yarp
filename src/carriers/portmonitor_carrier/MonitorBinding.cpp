// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "MonitorBinding.h"
#include "MonitorLua.h"

#include <yarp/os/ConstString.h>

using namespace yarp::os;

/**
 * Class MonitorBinding
 */

MonitorBinding::~MonitorBinding() 
{
} 

MonitorBinding* MonitorBinding::create(const char* script_type) 
{
    if(ConstString(script_type) == "lua")
        return new MonitorLua();
    return NULL;        
}


