#!/usr/bin/python

# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

import yarp

yarp.Network.init()

rf = yarp.ResourceFinder()
rf.setVerbose(True);
rf.setDefaultContext("myContext");
rf.setDefaultConfigFile("default.ini");

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

yarp.Network.fini();
