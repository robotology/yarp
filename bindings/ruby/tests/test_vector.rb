#!/usr/bin/env ruby

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

gem 'test-unit'
require 'test/unit'
require 'yarp'
include Test::Unit::Assertions


def test_vector()
  vecSize = 10
  vec = Yarp::Vector.new(vecSize)
  assert_equal(vec.size(), vecSize)
  vecCheck = Yarp::Vector.new(vec)
  assert_equal(vec.size(), vecCheck.size())

  # Check copy constructor from a Vector
  # returned by a function
  mat = Yarp::Matrix.new(3,3)
  mat.eye()
  vecTest = Yarp::Vector.new(mat.getRow(0))
  assert_equal(vecTest.size(), 3)
end

test_vector()
