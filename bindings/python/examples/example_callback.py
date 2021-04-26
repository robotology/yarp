#!/usr/bin/python3

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

import yarp

yarp.Network.init()

# alternative callback classes depending on the underlying data type:
# - yarp.PropertyCallback
# - yarp.TypedReaderCallbackImage(Rgb|Rgba|Mono|Mono16|Int|Float|RgbFloat)
# - yarp.TypedReaderCallbackVector (yarp.DVector)
# - yarp.TypedReaderCallbackSound
class CustomCallback(yarp.BottleCallback):
    def __init__(self):
        super().__init__()
        # remove this constructor if no class members need to be initialized,
        # keep the above parent constructor invocation otherwise

    def onRead(self, bot, reader):
        print("Port %s received: %s" % (reader.getName(), bot.toString()))

# alternative buffered port classes depending on the underlying data type:
# - yarp.BufferedPortProperty
# - yarp.BufferedPortImage(Rgb|Rgba|Mono|Mono16|Int|Float|RgbFloat)
# - yarp.BufferedPortVector (yarp.DVector)
# - yarp.BufferedPortSound
p = yarp.BufferedPortBottle()
c = CustomCallback()
p.useCallback(c)

print("Callback ready at port " + p.getName())
input("Press ENTER to quit\n")

p.interrupt()
p.close()

yarp.Network.fini()
