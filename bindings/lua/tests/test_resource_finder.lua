#!/usr/bin/lua

-- Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- All rights reserved.
--
-- This software may be modified and distributed under the terms of the
-- BSD-3-Clause license. See the accompanying LICENSE file for details.

require("yarp")

function test_resource_finder()
  -- initialize yarp network
  yarp.Network()

  local rf = yarp.ResourceFinder()
  rf:setDefaultContext("myContext")
  rf:setDefaultConfigFile("default.ini")
  rf:configure(arg)
end

test_resource_finder()
