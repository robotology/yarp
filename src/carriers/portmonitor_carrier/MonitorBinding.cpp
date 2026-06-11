/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MonitorBinding.h"
#include "MonitorSharedLib.h"

#ifdef ENABLED_PORTMONITOR_PYTHON
   #include "python/MonitorPython.h"
#endif

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
    if(std::string(script_type) == "lua")
    {
#ifdef ENABLED_PORTMONITOR_LUA
        return new MonitorLua();
#else
        yCError(PORTMONITORCARRIER) << "Cannot load LUA plugin. YARP portmonitor was built without LUA support";
        return nullptr;
#endif
    }
    else if(std::string(script_type) == "dll")
    {
        return new MonitorSharedLib();
    }
    else if(std::string(script_type) == "python")
    {
#ifdef ENABLED_PORTMONITOR_PYTHON
        return new MonitorPython();
#else
        yCError(PORTMONITORCARRIER) << "Cannot load PYTHON plugin. YARP portmonitor was built without PYTHON support";
        return nullptr;
#endif
    }
    else
    {
        yCError(PORTMONITORCARRIER, R"(Currently only 'lua','dll','python' objects are supported by portmonitor)");
    }


    return nullptr;
}
