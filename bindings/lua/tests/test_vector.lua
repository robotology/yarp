#!/usr/bin/lua

-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

require("yarp")

function test_vector()
  vecSize = 10
  vec = yarp.Vector(vecSize)
  assert(vec:size() == vecSize)
  vecCheck = yarp.Vector(vec)
  assert(vec:size() == vecCheck:size())

  -- Check copy constructor from a Vector
  -- returned by a function
  mat = yarp.Matrix(3,3)
  mat:eye()
  vecTest = yarp.Vector(mat:getRow(0))
  assert(vecTest:size() == 3)
end


test_vector()
