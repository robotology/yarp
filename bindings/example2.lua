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
sender = yarp.BufferedPortBottle()
receiver = yarp.BufferedPortBottle()
sender:open("/lua/write")
receiver:open("/lua/read")

-- connect sender to receiver
yarp.NetworkBase_connect(sender:getName():c_str(), receiver:getName():c_str())

for i=1,10 do

    -- write to the sender port
    local wb = sender:prepare()
    wb:clear()
    wb:addString("count")
    wb:addInt(i)
    wb:addString("of")
    wb:addInt(10)
    sender:write()

    -- read from receiver port
    local rb = receiver:read();
    print("Received: ", rb:toString())

    -- wait for a second
    yarp.Time_delay(1.0)
end

-- disconnect sender from receiver
yarp.NetworkBase_disconnect(sender:getName():c_str(), receiver:getName():c_str())

-- close the ports
sender:close()
receiver:close()

-- Deinitialize yarp network
yarp.Network_fini()

