#!/usr/bin/lua

-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

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
