// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <lua.hpp>
#include "MonitorLua.h"

using namespace yarp::os;
using namespace std;


/**
 * Class MonitorLua
 */
MonitorLua::MonitorLua(void) : bHasAcceptCallback(false), 
                               bHasUpdateCallback(false),
                               bHasUpdateReplyCallback(false)
{
    L = luaL_newstate();
    luaL_openlibs(L);

    /**
     * Registring extra functions for lua :
     *  - PortMonitor.setEvent()
     *  - PortMonitor.unsetEvent()
     *  - PortMonitor.setConstraint()
     *  - PortMonitor.getConstraint()
     */ 
    registerExtraFunctions();
}

MonitorLua::~MonitorLua()
{
    if(L)
    {
        //  call PortMonitor.destroy if exists
        if(getLocalFunction("destroy"))
        {
            if(lua_pcall(L, 0, 0, 0) != 0)
                yError(lua_tostring(L, -1));
        }
        // closing lua state handler
        lua_close(L);
    }        
}

bool MonitorLua::load(const Property &options)
{
    //printf("%s (%d)\n", __FILE__, __LINE__);
    if(luaL_loadfile(L, options.find("filename").asString().c_str()))
    {   
        yError("Cannot load script file");
        yError(lua_tostring(L, -1));
        lua_pop(L,1);
        lua_close(L);
        L = NULL;
        return false;
    }

    if(lua_pcall(L,0, LUA_MULTRET, 0))
    {
        yError("Cannot run script file");
        yError(lua_tostring(L, -1));
        lua_pop(L,1);
        lua_close(L);
        L = NULL;
        return false;
    }

    /**
     * TODO: make PortMonitor's element read only!
     */
    lua_pushlightuserdata(L, this);
    lua_setglobal(L, "PortMonitor_Owner");

    lua_getglobal(L, "PortMonitor");
    if(lua_istable(L, -1) == 0) 
    {
        yError("The script file does not contain any valid \'PortMonitor\' object.");
        lua_pop(L, 1);
        lua_close(L);
        L = NULL;
        return false;
    }

    bool result = true;
    //  call PortMonitor.create if exists
    if(getLocalFunction("create"))
    {
        // mapping to swig type
        swig_type_info *propType = SWIG_TypeQuery(L, "yarp::os::Property *");
        if(!propType)
        {
            yError("Swig type of Property is not found");
            lua_pop(L, 1);
            return false;
        }
        // getting the swig-type pointer
        SWIG_NewPointerObj(L, &options, propType, 0);
        if(lua_pcall(L, 1, 1, 0) != 0)
        {
            yError(lua_tostring(L, -1));
            lua_pop(L, 1);
            lua_close(L);
            L = NULL;
            return false;
        }
        else    
            result = lua_toboolean(L, -1);
    }  
    lua_pop(L,1);
    
    // Check if there is accept callback
    bHasAcceptCallback = getLocalFunction("accept");
    lua_pop(L,1);

    // Check if there is update callback
    bHasUpdateCallback = getLocalFunction("update");
    lua_pop(L,1);

    // Check if there is update callback
    bHasUpdateReplyCallback = getLocalFunction("update_reply");
    lua_pop(L,1);

    return result;
}

bool MonitorLua::acceptData(Things &thing)
{
    if(getLocalFunction("accept"))
    {
        // mapping to swig type
        swig_type_info *thingsType = SWIG_TypeQuery(L, "yarp::os::Things *");
        if(!thingsType)
        {            
            yError("Swig type of Things is not found");
            lua_pop(L, 1);
            return false;
        }
        
        // getting the swig-type pointer
        SWIG_NewPointerObj(L, &thing, thingsType, 0);
        if(lua_pcall(L, 1, 1, 0) != 0)
        {
            yError(lua_tostring(L, -1));
            lua_pop(L, 1);
            return false;
        }
            
        // converting the results        
        bool result = lua_toboolean(L, -1);
        lua_pop(L, 1);
        return result;        
    }

    lua_pop(L, 1);
    return true;
}


yarp::os::Things& MonitorLua::updateData(yarp::os::Things& thing)
{
    if(getLocalFunction("update"))
    {
        // mapping to swig type
        swig_type_info *thingsType = SWIG_TypeQuery(L, "yarp::os::Things *");        
        if(!thingsType)
        {            
            yError("Swig type of Things is not found");
            lua_pop(L, 1);
            return thing;
        }
        
        // getting the swig-type pointer
        SWIG_NewPointerObj(L, &thing, thingsType, 0);
        if(lua_pcall(L, 1, 1, 0) != 0)
        {
            yError(lua_tostring(L, -1));
            lua_pop(L, 1);
            return thing;
        }

        // converting the results
        yarp::os::Things* result;
        if(SWIG_Lua_ConvertPtr(L, -1, (void**)(&result), thingsType, 0) != SWIG_OK )
        {
            yError("Cannot get a valid return value from PortMonitor.update");
            lua_pop(L, 1);
            return thing;
        }   
        else        
        {
            lua_pop(L, 1);
            return *result;
        }
    }

    lua_pop(L,1);
    return thing;
}

yarp::os::Things& MonitorLua::updateReply(yarp::os::Things& thing)
{
    if(getLocalFunction("update_reply"))
    {
        // mapping to swig type
        swig_type_info *thingsType = SWIG_TypeQuery(L, "yarp::os::Things *");
        if(!thingsType)
        {
            yError("Swig type of Things is not found");
            lua_pop(L, 1);
            return thing;
        }

        // getting the swig-type pointer
        SWIG_NewPointerObj(L, &thing, thingsType, 0);
        if(lua_pcall(L, 1, 1, 0) != 0)
        {
            yError(lua_tostring(L, -1));
            lua_pop(L, 1);
            return thing;
        }

        // converting the results
        yarp::os::Things* result;
        if(SWIG_Lua_ConvertPtr(L, -1, (void**)(&result), thingsType, 0) != SWIG_OK )
        {
            yError("Cannot get a valid return value from PortMonitor.update_reply");
            lua_pop(L, 1);
            return thing;
        }
        else
        {
            lua_pop(L, 1);
            return *result;
        }
    }

    lua_pop(L,1);
    return thing;
}

bool MonitorLua::setParams(const yarp::os::Property& params)
{
    if(getLocalFunction("setparam"))
    {
        // mapping to swig type
        swig_type_info *propType = SWIG_TypeQuery(L, "yarp::os::Property *");
        if(!propType)
        {            
            yError("Swig type of Property is not found");
            lua_pop(L, 1);
            return false;
        }
        
        // getting the swig-type pointer
        SWIG_NewPointerObj(L, &params, propType, 0);
        if(lua_pcall(L, 1, 0, 0) != 0)
        {
            yError(lua_tostring(L, -1));
            lua_pop(L, 1);
            return false;
        }
        return true;
    }

    lua_pop(L,1);
    return true;
}

bool MonitorLua::getParams(yarp::os::Property& params)
{
    if(getLocalFunction("getparam"))
    {
        // mapping to swig type
        swig_type_info *propType = SWIG_TypeQuery(L, "yarp::os::Property *");
        if(!propType)
        {            
            yError("Swig type of Property is not found");
            lua_pop(L, 1);
            return false;
        }
        
        // calling PortMonitor.getparam from lua
        if(lua_pcall(L, 0, 1, 0) != 0)
        {
            yError(lua_tostring(L, -1));
            lua_pop(L, 1);
            return false;
        }

        // converting the results
        yarp::os::Property* result;
        if(SWIG_Lua_ConvertPtr(L, -1, (void**)(&result), propType, 0) != SWIG_OK )
        {
            yError("Cannot get a valid return value from PortMonitor.getparam");
            lua_pop(L, 1);
            return false;
        }   
        else        
        {
            params = *result;
            lua_pop(L, 1);
            return true;
        }
    }

    lua_pop(L,1);
    return true;
}

bool MonitorLua::peerTrigged(void)
{
    if(getLocalFunction("trig"))
    {
        if(lua_pcall(L, 0, 0, 0) != 0)
        {
            yError(lua_tostring(L, -1));
            lua_pop(L, 1);
            return false;
        }
        return true;
    }

    lua_pop(L, 1);
    return true;
}


bool MonitorLua::getLocalFunction(const char *name) 
{
  lua_pushstring(L, name);
  lua_gettable(L, -2);
  return (lua_isfunction(L, -1) == 1);
}


bool MonitorLua::registerExtraFunctions(void)
{
#if LUA_VERSION_NUM > 501
    lua_newtable(L);
    luaL_setfuncs (L, MonitorLua::portMonitorLib, 0);
    lua_pushvalue(L, -1); 
    lua_setglobal(L, "PortMonitor"); 
#else
    // deprecated 
    //luaL_openlib(L, "PortMonitor", MonitorLua::portMonitorLib, 0);
    luaL_register(L, "PortMonitor", MonitorLua::portMonitorLib);   
#endif
    return true;
}


bool MonitorLua::canAccept(void)
{
    if(constraint == "")
        return true;

    MonitorEventRecord& record = MonitorEventRecord::getInstance();
   
    /**
     * following piece of code replaces each event symbolic name
     * with a boolean value based on their existance in MonitorEventRecord 
     */
    string strConstraint = constraint;
    string strDummy = strConstraint;
    searchReplace(strDummy, "(", " ");
    searchReplace(strDummy, ")", " ");
    // wrap it with some  guard space
    strDummy = " " + strDummy + " "; 
    string delimiter = " ";
    size_t pos = 0;
    string token;
    while ((pos = strDummy.find(delimiter)) != string::npos) 
    {
        token = strDummy.substr(0, pos);
        if(token.size() && !isKeyword(token.c_str()))
        {    
            record.lock();
            string value = (record.hasEvent(token.c_str())) ? "true" : "false";
            record.unlock();
            searchReplace(strConstraint, token, value);
        }
        strDummy.erase(0, pos + delimiter.length());
    }
    //printf("constraint = \'%s\'\n", strConstraint.c_str());

    /*
     *  Using lua to evaluate the boolean expression
     *  Note: this can be replaced by a homebrew boolean 
     *  expression validator (e.g., BinNodeType from libyarpmanager)
     */
    strConstraint = "return " + strConstraint;

    if(luaL_dostring(L, strConstraint.c_str()) != 0)
    {        
        yError(lua_tostring(L, -1));
        return false;
    }
    
    if(!lua_isboolean(L, -1))
    {
        yError(lua_tostring(L, -1));
        return false;      
    }
    
    bool accepted = (lua_toboolean(L,-1) == 1);
    lua_pop(L, 1);
    return accepted;
}


inline void MonitorLua::searchReplace(string& str, const string& oldStr, const string& newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != string::npos)
  {
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

inline void MonitorLua::trimString(string& str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

inline bool MonitorLua::isKeyword(const char* str) 
{    
    if(!str)
        return false;

    string token = str;
    if((token == "true") || (token == "false") ||
       (token == "and") || (token == "or") || (token == "not") )
       return true;
    return false;
}


/**
 * static members 
 */

int MonitorLua::setConstraint(lua_State* L)
{
    const char *cst = luaL_checkstring(L, 1);
    if(cst)
    {
        lua_getglobal(L, "PortMonitor_Owner");
        if(!lua_islightuserdata(L, -1))
        {
            yError("Cannot get PortMonitor_Owner");
            return 0;
        }

        MonitorLua* owner = static_cast<MonitorLua*>(lua_touserdata(L, -1));
        yAssert(owner);
        owner->setAcceptConstraint(cst);
    }        
    return 0;
}

int MonitorLua::getConstraint(lua_State* L)
{
    lua_getglobal(L, "PortMonitor_Owner");
    if(!lua_islightuserdata(L, -1))
    {
        yError("Cannot get PortMonitor_Owner");
        return 0;
    }

    MonitorLua* owner = static_cast<MonitorLua*>(lua_touserdata(L, -1));
    yAssert(owner);
    lua_pushstring(L, owner->getAcceptConstraint());
    return 0;
}


int MonitorLua::setEvent(lua_State* L)
{
    double lifetime = -1.0;
    int n_args =  lua_gettop(L); 
    const char *event_name = luaL_checkstring(L, 1);
    if(event_name)
    {
        // check if the event's lifetime is given as argument
        if(n_args > 1)                          
        {
            if(lua_isnumber(L,2))
                lifetime = (double) luaL_checknumber(L,2);
            else
            {
                yError("The second arguemnt of setEvent() must be number");
                return 0;
            }
        }

        lua_getglobal(L, "PortMonitor_Owner");
        if(!lua_islightuserdata(L, -1))
        {
            yError("Cannot get PortMonitor_Owner");
            return 0;
        }
        MonitorLua* owner = static_cast<MonitorLua*>(lua_touserdata(L, -1));
        yAssert(owner);
        if(owner->isKeyword(event_name))
            return 0;
        MonitorEventRecord& record = MonitorEventRecord::getInstance();
        record.lock();
        record.setEvent(event_name, owner, lifetime);
        record.unlock();
    }        
    return 0;
}

int MonitorLua::unsetEvent(lua_State* L)
{
    const char *event_name = luaL_checkstring(L, 1);
    if(event_name)
    {
        lua_getglobal(L, "PortMonitor_Owner");
        if(!lua_islightuserdata(L, -1))
        {
            yError("Cannot get PortMonitor_Owner");
            return 0;
        }
        MonitorLua* owner = static_cast<MonitorLua*>(lua_touserdata(L, -1));
        yAssert(owner);
        if(owner->isKeyword(event_name))
            return 0;
        MonitorEventRecord& record = MonitorEventRecord::getInstance();
        record.lock();
        record.unsetEvent(event_name, owner);
        record.unlock();
    }        
    return 0;
}

#if LUA_VERSION_NUM > 501
const struct luaL_Reg MonitorLua::portMonitorLib [] = {
#else
const struct luaL_reg MonitorLua::portMonitorLib [] = {
#endif
    {"setConstraint", MonitorLua::setConstraint},
    {"getConstraint", MonitorLua::getConstraint},
    {"setEvent", MonitorLua::setEvent},
    {"unsetEvent", MonitorLua::unsetEvent},
    {NULL, NULL}
};


