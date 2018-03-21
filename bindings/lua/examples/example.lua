#!/usr/bin/lua 

-- Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
-- All rights reserved.
--
-- This software may be modified and distributed under the terms of the
-- BSD-3-Clause license. See the accompanying LICENSE file for details.


-- LUA_CPATH should have the path to yarp-lua binding library (i.e. yarp.so, yarp.dll) 
require("yarp")

-- initialize yarp network
yarp.Network()

local rf = yarp.ResourceFinder()
rf:setVerbose(true)
rf:setDefaultContext("myContext")
rf:setDefaultConfigFile("default.ini")
rf:configure(arg)

-- call static member in this way
-- t = yarp.Time_now()

-- create and open sender/receiver ports
port = yarp.BufferedPortBottle()
port:open("/lua")

for i=1,100 do
    -- write to the port
    local wb = port:prepare()
    wb:clear()
    wb:addString("count")
    wb:addInt(i)
    wb:addString("of")
    wb:addInt(100)
    print(string.format("Sending: %s", wb:toString()))
    port:write()
    yarp.Time_delay(0.5)
end

-- close the port
port:close()

-- Deinitialize yarp network
yarp.Network_fini()

