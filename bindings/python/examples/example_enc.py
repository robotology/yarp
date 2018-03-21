#!/usr/bin/python

# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

import sys
import yarp

def main():
    yarp.Network.init()

    options = yarp.Property()
    driver = yarp.PolyDriver()

    # set the poly driver options 
    options.put("robot", "icub")
    options.put("device", "remote_controlboard")
    options.put("local", "/example_enc/client")
    options.put("remote", "/icub/left_arm")

    # opening the drivers
    print 'Opening the motor driver...'
    driver.open(options)
    if not driver.isValid():
        print 'Cannot open the driver!'
        sys.exit()

    # opening the drivers
    print 'Viewing motor position/encoders...'
    ipos = driver.viewIPositionControl()
    ienc = driver.viewIEncoders()
    if ienc is None or ipos is None:
        print 'Cannot view motor positions/encoders!'
        sys.exit()
     
    # wait a bit for the interface 
    yarp.Time_delay(1.0)

    encs = yarp.Vector(ipos.getAxes())
    for i in range(0,10):
        # read encoders
        ret = ienc.getEncoders(encs.data())
        if ret is False: continue

        print "Current encoders value: "
        print encs.toString(-1, -1)
        yarp.Time_delay(0.01)
    
    # closing the driver
    driver.close()
    yarp.Network.fini()

if __name__ == "__main__":
    main()

