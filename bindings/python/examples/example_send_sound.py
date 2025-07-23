#!/usr/bin/python3

# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

import yarp

yarp.Network.init()

p = yarp.BufferedPortSound()
p.open("/python:o");

top = 100;
for i in range(1,top):
    snd = p.prepare()
    snd.setFrequency(16000);
    snd.resize(32000,2);
    p.write()
    yarp.delay(0.5)
p.close();

yarp.Network.fini();
