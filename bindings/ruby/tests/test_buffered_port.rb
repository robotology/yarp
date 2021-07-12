#!/usr/bin/env ruby

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

gem 'test-unit'
require 'test/unit'
require 'yarp'
include Test::Unit::Assertions

def open_and_connect()
  p = Yarp::BufferedPortBottle.new()
  p2 = Yarp::BufferedPortBottle.new()
  assert(p.open("/ruby/out"))
  assert_equal(true, p2.open("/ruby/in"))
  assert_equal(true,Yarp::Network::connect(p.getName(), p2.getName()))
  p.close()
  p2.close()
end
def open_and_connect_comm()
  p = Yarp::BufferedPortBottle.new()
  p2 = Yarp::BufferedPortBottle.new()
  bt_out = p.prepare()
  bt_out.addInt32(10)
  assert_equal(true, p.open("/ruby/out"))
  assert_equal(true, p2.open("/ruby/in"))
  assert_equal(true,Yarp::Network::connect(p.getName(), p2.getName()))
  Yarp::delay(0.5)
  p.write()
  Yarp::delay(0.5)
  bt_in=p2.read()
  assert_equal(1, bt_in.size())
  assert_equal(10, bt_in.get(0).asInt32())
  p.close()
  p2.close()
end

Yarp::Network.init()
Yarp::Network.setLocalMode(true)

open_and_connect()
open_and_connect_comm()
