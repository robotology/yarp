// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MONITORLUA_INC_
#define _MONITORLUA_INC_

#include <string>
#include <yarp/os/ConstString.h>
#include "MonitorBinding.h"
#include "lua_swig.h"

class MonitorLua : public MonitorBinding
{

public:
    MonitorLua(void);
    virtual ~MonitorLua();
    
    bool loadScript(const char* script_file);
    bool setParams(const yarp::os::Property& params);
    bool getParams(yarp::os::Property& params);
    bool acceptData(yarp::os::ConnectionReader& reader);
    yarp::os::ConnectionReader& updateData(yarp::os::ConnectionReader& reader);    
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

private:
    lua_State *L;   
    std::string constraint;

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
#if LUA_VERSION_NUM > 501
    static const struct luaL_Reg portMonitorLib[];
#else
    static const struct luaL_reg portMonitorLib[];
#endif

};

#endif //_MONITORLUA_INC_


