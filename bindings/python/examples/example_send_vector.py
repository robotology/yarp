#!/usr/bin/python3

# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

import yarp

yarp.Network.init()

p = yarp.BufferedPortVector()
p.open("/python:o");

top = 100;
for i in range(1,top):
    vec = p.prepare()
    vec.resize(3)
    vec[0]=0
    vec[1]=1
    vec[2]=2
    p.write()
    yarp.delay(0.5)
p.close();

yarp.Network.fini();
