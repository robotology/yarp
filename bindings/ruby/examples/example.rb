#!/usr/bin/ruby

# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

require 'yarp'

Yarp::Network.init()

p = Yarp::BufferedPortBottle.new()
p.open("/ruby")

top = 100
for i in 1..top do
    bottle = p.prepare()
    bottle.clear()
    bottle.addString("count")
    bottle.addInt(i)
    bottle.addString("of")
    bottle.addInt(top)
    puts "Sending", bottle.toString()
    p.write()
    Yarp::Time.delay(0.5)
end

p.close()

Yarp::Network.fini()
