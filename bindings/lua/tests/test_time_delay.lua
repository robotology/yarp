#!/usr/bin/lua

-- Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- All rights reserved.
--
-- This software may be modified and distributed under the terms of the
-- BSD-3-Clause license. See the accompanying LICENSE file for details.

require("yarp")

function test_time_delay()
  yarp.Network()
  yarp.Network.setLocalMode(true)
  first = yarp.now()
  delay = 1.0
  yarp.delay(delay)
  last = yarp.now()
end

test_time_delay()
