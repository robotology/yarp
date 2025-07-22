#!/usr/bin/python3

# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

import yarp

yarp.Network.init()

class CustomCallback(yarp.MatrixCallback):
    def __init__(self):
        super().__init__()
        # remove this constructor if no class members need to be initialized,
        # keep the above parent constructor invocation otherwise

    def onRead(self, mat, reader):
        print("Port %s received: \n%s" % (reader.getName(), mat.toString(3,3)))

p = yarp.BufferedPortMatrix()

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
