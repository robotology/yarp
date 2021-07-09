#!/usr/bin/lua

-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

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
