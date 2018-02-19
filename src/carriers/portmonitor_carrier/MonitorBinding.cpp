/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "MonitorBinding.h"
#include "MonitorSharedLib.h"

#ifdef ENABLED_PORTMONITOR_LUA
    #include "MonitorLua.h"
#endif

#include <yarp/os/ConstString.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;

/**
 * Class MonitorBinding
 */

MonitorBinding::~MonitorBinding() 
{
} 

MonitorBinding* MonitorBinding::create(const char* script_type)
{
    if(ConstString(script_type) == "lua") {
#ifdef ENABLED_PORTMONITOR_LUA
        return new MonitorLua();
#else
        yError()<<"Cannot load LUA plugin. YARP portmonitor was built without LUA support";
        return NULL;
#endif
    }

    if(ConstString(script_type) == "dll")
        return new MonitorSharedLib();
       
    return nullptr;        
}
