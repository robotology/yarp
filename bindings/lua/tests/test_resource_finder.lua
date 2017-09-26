#!/usr/bin/lua

-- Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
-- Author: Silvio Traversaro
-- Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

require("yarp")

function test_resource_finder()
  -- initialize yarp network
  yarp.Network()
  
  local rf = yarp.ResourceFinder()
  rf:setVerbose(true)
  rf:setDefaultContext("myContext")
  rf:setDefaultConfigFile("default.ini")
  rf:configure(arg)
end

test_resource_finder()

