#!/usr/bin/python

# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

import yarp

yarp.Network.init()

prop = yarp.Property()
driver = yarp.PolyDriver()
ienc = yarp.IEncoders()

# opening the drivers

''''
p = yarp.BufferedPortBottle()
p.open("/python");

top = 100;
for i in range(1,top):
    bottle = p.prepare()
    bottle.clear()
    bottle.addString("count")
    bottle.addInt(i)
    bottle.addString("of")
    bottle.addInt(top)
    print ("Sending", bottle.toString())
    p.write()
    yarp.Time.delay(0.5)

p.close();
'''

yarp.Network.fini();
