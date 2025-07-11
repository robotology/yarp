#!/usr/bin/python3

# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

import yarp

yarp.Network.init()

p = yarp.BufferedPortMatrix()
p.open("/python");

top = 100;
for i in range(1,top):
    mat = p.prepare()
    mat.resize(2,2)
    vec[0][0]=0
    vec[1][0]=1
    vec[0][1]=2
    mat[1][1]=3
    p.write()
    yarp.delay(0.5)
p.close();

yarp.Network.fini();
