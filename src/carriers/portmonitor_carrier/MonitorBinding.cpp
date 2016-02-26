/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
       
    return NULL;        
}
