#!/usr/bin/python3

# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

import yarp

yarp.Network.init()

p = yarp.BufferedPortMatrix()
p.open("/python:o");

top = 100;
for i in range(1,top):
    mat = p.prepare()
    mat.resize(2,2)
    mat.set(0,0,0)
    mat.set(0,1,1)
    mat.set(1,0,2)
    mat[1,1]=3 # just a different way to assign
    p.write()
    yarp.delay(0.5)
p.close();

yarp.Network.fini();
