/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MONITORLUA_INC
#define MONITORLUA_INC

#include <string>
#include <string>
#include "MonitorBinding.h"
#include "swigluarun.h"
#include <mutex>

class MonitorLua : public MonitorBinding
{

public:
    MonitorLua();
    ~MonitorLua() override;

    bool load(const yarp::os::Property& options) override;
    bool setParams(const yarp::os::Property& params) override;
    bool getParams(yarp::os::Property& params) override;

    bool acceptData(yarp::os::Things& thing) override;
    yarp::os::Things& updateData(yarp::os::Things& thing) override;
    yarp::os::Things& updateReply(yarp::os::Things& thing) override;

    bool peerTrigged() override;
    bool canAccept() override;

    bool setAcceptConstraint(const char* constraint) override {
        if(!constraint) {
            return false;
        }
        MonitorLua::constraint = constraint;
        trimString(MonitorLua::constraint);
        return true;
    }

    const char* getAcceptConstraint() override {
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
    std::recursive_mutex luaMutex;

public:
    MonitorTrigger<MonitorLua>* trigger;

private:
    bool getLocalFunction(const char *name);
    bool registerExtraFunctions();
    void trimString(std::string& str);
    void searchReplace(std::string& str,
                       const std::string& oldStr, const std::string& newStr);
    bool isKeyword(const char* str);

    /* lua accessible function*/
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


#endif //_MONITORLUA_INC_
