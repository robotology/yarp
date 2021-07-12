#!/usr/bin/lua

-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

require("yarp")

function test_bottle()
  bt = yarp.Bottle()
  bt:addString("str")
  assert(bt:toString() == "str")

  bt:clear()
  integerVal = 10
  bt:addInt32(integerVal)
  assert("number"==type(bt:get(0):asInt8()))
  assert(integerVal==bt:get(0):asInt8())
  assert("number"==type(bt:get(0):asInt16()))
  assert(integerVal==bt:get(0):asInt16())
  assert("number"==type(bt:get(0):asInt32()))
  assert(integerVal==bt:get(0):asInt32())
  assert("number"==type(bt:get(0):asInt64()))
  assert(integerVal==bt:get(0):asInt64())

  floatVal = 10.0
  bt:clear()
  bt:addFloat32(floatVal)
  assert("number"==type(bt:get(0):asFloat32()))
  assert(floatVal==bt:get(0):asFloat32())
  assert("number"==type(bt:get(0):asFloat64()))
  assert(floatVal==bt:get(0):asFloat64())

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
