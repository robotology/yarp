#!/usr/bin/lua

-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

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
