#!/usr/bin/lua

-- Copyright: (C) 2013 Istituto Italiano di Tecnologia (IIT)
-- Author: Paul Fitzpatrick, Juan G Victores
-- Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

require("yarp")

function test_port()
  yarp.Network()
  yarp.Network.setLocalMode(true)
  port = yarp.BufferedPortBottle()
  port:open("/lua/test")
  port:close()
end

test_port()
