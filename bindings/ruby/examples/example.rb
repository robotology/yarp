#!/usr/bin/ruby

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

require 'yarp'

Yarp::Network.init()

p = Yarp::BufferedPortBottle.new()
p.open("/ruby")

top = 100
for i in 1..top do
    bottle = p.prepare()
    bottle.clear()
    bottle.addString("count")
    bottle.addInt32(i)
    bottle.addString("of")
    bottle.addInt32(top)
    puts "Sending", bottle.toString()
    p.write()
    Yarp::Time.delay(0.5)
end

p.close()

Yarp::Network.fini()
