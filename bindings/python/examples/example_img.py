#!/usr/bin/python

# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

import yarp

yarp.Network.init()

p = yarp.BufferedPortImageRgb()
p.open("/python");

top = 100;
for i in range(1,top):
    img = p.prepare()
    img.resize(320,240)
    img.zero()
    img.pixel(160,120).r = 255
    p.write()
    yarp.Time.delay(0.5)
p.close();

yarp.Network.fini();
