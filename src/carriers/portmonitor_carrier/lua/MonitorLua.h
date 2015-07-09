// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MONITORLUA_INC_
#define _MONITORLUA_INC_

#include <string>
#include <yarp/os/ConstString.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/RecursiveMutex.h>
#include "MonitorBinding.h"
#include "lua_swig.h"

class MonitorTrigger;

class MonitorLua : public MonitorBinding
{

public:
    MonitorLua(void);
    virtual ~MonitorLua();

    bool load(const yarp::os::Property& options);
    bool setParams(const yarp::os::Property& params);
    bool getParams(yarp::os::Property& params);

    bool acceptData(yarp::os::Things& thing);
    yarp::os::Things& updateData(yarp::os::Things& thing);
    yarp::os::Things& updateReply(yarp::os::Things& thing);

    bool peerTrigged(void);
    bool canAccept(void);

    bool setAcceptConstraint(const char* constraint) {
        if(!constraint)
            return false;        
        MonitorLua::constraint = constraint;
        trimString(MonitorLua::constraint);
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

    bool hasUpdateReply() {
        return bHasUpdateReplyCallback;
    }

private:
    lua_State *L;   
    std::string constraint;
    bool bHasAcceptCallback;
    bool bHasUpdateCallback;
    bool bHasUpdateReplyCallback;
    yarp::os::RecursiveMutex luaMutex;

public:
    MonitorTrigger* trigger;

private:
    bool getLocalFunction(const char *name);
    bool registerExtraFunctions(void); 
    void trimString(std::string& str);
    void searchReplace(std::string& str, 
                       const std::string& oldStr, const std::string& newStr);
    bool isKeyword(const char* str);

    /* lua accessible fucntion*/
    static int setConstraint(lua_State* L);
    static int getConstraint(lua_State* L);
    static int setEvent(lua_State* L); 
    static int unsetEvent(lua_State* L); 
    static int setTrigInterval(lua_State* L);
#if LUA_VERSION_NUM > 501
    static const struct luaL_Reg portMonitorLib[];
#else
    static const struct luaL_reg portMonitorLib[];
#endif

};

class MonitorTrigger : public yarp::os::RateThread {
public:
    MonitorTrigger(MonitorLua* monitor, int period)
        : yarp::os::RateThread(period) {
        MonitorTrigger::monitor = monitor;
    }
    virtual ~MonitorTrigger() { }

    // inherited from the yarp::os::RateThread
    virtual void run () {
        monitor->peerTrigged();
    }

private:
    MonitorLua* monitor;
};


#endif //_MONITORLUA_INC_


