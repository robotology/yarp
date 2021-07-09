#!/usr/bin/lua

-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

require("yarp")

yarp.Network.init()
yarp.Network.setLocalMode(true)


function imageTest()
  port_in = yarp.Port()
  in_port_name = "/test/img:i"
  assert(port_in:open(in_port_name))
  port_out = yarp.Port()
  port_out:enableBackgroundWrite(true);
  out_port_name = "/test/img:o"
  assert(port_out:open(out_port_name))
  height = 240
  width = 320
  yarp_img_out = yarp.ImageMono()
  yarp_img_out:resize(width, height)
  yarp_img_in = yarp.ImageMono()
  assert(240 == yarp_img_out:height())
  assert(320 == yarp_img_out:width())
  yarp_vector = yarp.Vector()
  yarp_vector:resize(320*240, 0)
  assert(240*320 == yarp_vector:size())
  yarp_img_out:setExternal(yarp_vector, width, height)
  assert(yarp.Network.connect(port_out:getName(), port_in:getName()))
  yarp.delay(0.5)
  assert(port_out:write(yarp_img_out))
  yarp.delay(0.5)
  assert(port_in:read(yarp_img_in))
  assert(240 == yarp_img_in:height())
  assert(320 == yarp_img_in:width())

  port_out:close()
  port_in:close()

end

imageTest()
