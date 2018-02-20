#!/usr/bin/lua

-- Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
-- All rights reserved.
--
-- This software may be modified and distributed under the terms of the
-- BSD-3-Clause license. See the accompanying LICENSE file for details.

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


