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


-- call static member in this way
-- t = yarp.Time_now()

-- create and open sender/receiver ports
sender = yarp.BufferedPortBottle()
receiver = yarp.BufferedPortBottle()
sender:open("/lua/write")
receiver:open("/lua/read")

-- connect sender to receiver
yarp.NetworkBase_connect(sender:getName(), receiver:getName())

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
yarp.NetworkBase_disconnect(sender:getName(), receiver:getName())

-- close the ports
sender:close()
receiver:close()

-- Deinitialize yarp network
yarp.Network_fini()

