#!/usr/bin/python3

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

import yarp

yarp.Network.init()

class CustomCallback(yarp.ImageRgbCallback):
    def __init__(self):
        super().__init__()
        # remove this constructor if no class members need to be initialized,
        # keep the above parent constructor invocation otherwise

    def onRead(self, img, reader):
        print("Port %s received an image with size: %d %d" % (reader.getName(), img.width(), img.height() ))

p = yarp.BufferedPortImageRgb()

port_name = "/python:i"
p.open(port_name)

if not yarp.Network.connect("/python:o", port_name):
    print("Not connected")
    exit(1)

c = CustomCallback()
p.useCallback(c)

print("Callback ready at port " + p.getName())
input("Press ENTER to quit\n")

p.interrupt()
p.close()

yarp.Network.fini()
