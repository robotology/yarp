#!/usr/bin/env ruby

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

gem 'test-unit'
require 'test/unit'
require 'yarp'
include Test::Unit::Assertions


def test_image()
  p = Yarp::Port.new()
  p2 = Yarp::Port.new()
  p.enableBackgroundWrite(true)
  assert_equal(true, p.open("/ruby/out"))
  assert_equal(true, p2.open("/ruby/in"))
  assert_equal(true,Yarp::Network::connect(p.getName(), p2.getName()))
  Yarp::delay(0.5)
  height = 240
  width = 320
  yarp_img_out = Yarp::ImageMono.new()
  yarp_img_out.resize(width, height)
  yarp_img_in = Yarp::ImageMono.new()
  assert_equal(240, yarp_img_out.height())
  assert_equal(320, yarp_img_out.width())
  yarp_vector = Yarp::Vector.new()
  yarp_vector.resize(320*240, 0)
  assert_equal(240*320, yarp_vector.size())
  yarp_img_out.setExternal(yarp_vector, width, height)
  assert_equal(true, p.write(yarp_img_out))
  Yarp::delay(0.5)
  assert_equal(true, p2.read(yarp_img_in))
  assert_equal(240, yarp_img_in.height())
  assert_equal(320, yarp_img_in.width())
  p.close()
  p2.close()
end


Yarp::Network.init()
Yarp::Network.setLocalMode(true)

test_image()
