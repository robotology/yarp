#!/usr/bin/ruby

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

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
