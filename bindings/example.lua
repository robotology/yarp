#!/usr/bin/lua 

-- Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
-- Author: Ali Paikan <ali.paikan@iit.it>
-- Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


-- LUA_CPATH should have the path to yarp-lua binding library (i.e. yarp.so, yarp.dll) 
require("yarp")

-- initialize yarp network
yarp.Network()


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
    port:write()
    yarp.Time_delay(0.5)
end

-- close the port
port:close()

-- Deinitialize yarp network
yarp.Network_fini()

