#!/usr/bin/lua

-- Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
-- Author: Silvio Traversaro
-- Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

require("yarp")

function test_bottle()
  bot = yarp.Bottle()
  bot:addString("str")
  assert(bot:toString() == "str")
end

function test_property()
  prop = yarp.Property()
  assert(not prop:check("key1"))
  prop:put("key1", "value1")
  assert(prop:check("key1"))
  assert(not prop:check("key2"))
end

test_bottle()
test_property()


