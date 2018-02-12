#!/usr/bin/lua

-- Copyright: (C) 2013 Istituto Italiano di Tecnologia (IIT)
-- Author: Paul Fitzpatrick
-- Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

-- set LUA_CPATH to include yarp.so/yarp.dll
require("yarp")
yarp.Network()
port = yarp.BufferedPortBottle()
port:open("/lua/test")
port:close()
