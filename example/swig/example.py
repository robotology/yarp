#!/usr/bin/python

import yarp

yarp.Network.init()

p = yarp.BufferedPortBottle()
p.open("/python");
yarp.Network.connect("/foo","/bar");

top = 100;
for i in range(1,top):
    bottle = p.prepare()
    bottle.clear()
    bottle.addString("count")
    bottle.addInt(i)
    bottle.addString("of")
    bottle.addInt(top)
    print "Sending", bottle.toString()
    p.write()
    yarp.Time.delay(0.5)

p.close();

yarp.Network.fini();
