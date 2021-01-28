#!/usr/bin/lua

-- Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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
port = yarp.RpcClient()
port:open("/client")

yarp.NetworkBase_connect("/client", "/server", "tcp+send.portmonitor+file.rpc_modifier")

for i=1,100 do
    cmd = yarp.Bottle()
    rep = yarp.Bottle()
    cmd:addString("Hello")
    print("command:", cmd:toString())
    port:write(cmd, rep)
    print("reply:  ", rep:toString())
    yarp.Time_delay(0.5)
end

-- close the port
port:close()

-- Deinitialize yarp network
yarp.Network_fini()
