#!/usr/bin/ruby

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
