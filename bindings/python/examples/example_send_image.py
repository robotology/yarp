#!/usr/bin/python3

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

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
    yarp.delay(0.5)
p.close();

yarp.Network.fini();
