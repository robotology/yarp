#!/usr/bin/lua

-- Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- All rights reserved.
--
-- This software may be modified and distributed under the terms of the
-- BSD-3-Clause license. See the accompanying LICENSE file for details.

require("yarp")

function test_port()
  yarp.Network()
  yarp.Network.setLocalMode(true)
  port = yarp.BufferedPortBottle()
  assert(port:open("/lua/test"))
  port:close()
  port_name = "/lua/test"
  assert(port:open(port_name))
  port:close()
end

test_port()
