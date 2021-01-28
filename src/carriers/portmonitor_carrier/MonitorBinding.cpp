/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "MonitorBinding.h"
#include "MonitorSharedLib.h"
#include "MonitorLogComponent.h"

#ifdef ENABLED_PORTMONITOR_LUA
    #include "MonitorLua.h"
#endif

#include <string>
#include <yarp/os/LogStream.h>

using namespace yarp::os;

/**
 * Class MonitorBinding
 */

MonitorBinding::~MonitorBinding() = default;

MonitorBinding* MonitorBinding::create(const char* script_type)
{
    if(std::string(script_type) == "lua") {
#ifdef ENABLED_PORTMONITOR_LUA
        return new MonitorLua();
#else
        yCError(PORTMONITORCARRIER) << "Cannot load LUA plugin. YARP portmonitor was built without LUA support";
        return nullptr;
#endif
    }

    if(std::string(script_type) == "dll") {
        return new MonitorSharedLib();
    }

    return nullptr;
}
