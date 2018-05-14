/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef MONITORLUA_INC
#define MONITORLUA_INC

#include <string>
#include <string>
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

    bool load(const yarp::os::Property& options) override;
    bool setParams(const yarp::os::Property& params) override;
    bool getParams(yarp::os::Property& params) override;

    bool acceptData(yarp::os::Things& thing) override;
    yarp::os::Things& updateData(yarp::os::Things& thing) override;
    yarp::os::Things& updateReply(yarp::os::Things& thing) override;

    bool peerTrigged(void) override;
    bool canAccept(void) override;

    bool setAcceptConstraint(const char* constraint) override {
        if(!constraint)
            return false;
        MonitorLua::constraint = constraint;
        trimString(MonitorLua::constraint);
        return true;
    }

    const char* getAcceptConstraint(void) override {
        return constraint.c_str();
    }

    bool hasAccept() override {
        return bHasAcceptCallback;
    }

    bool hasUpdate() override {
        return bHasUpdateCallback;
    }

    bool hasUpdateReply() override {
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
    virtual void run () override {
        monitor->peerTrigged();
    }

private:
    MonitorLua* monitor;
};


#endif //_MONITORLUA_INC_


