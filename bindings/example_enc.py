#!/usr/bin/python

# Copyright: (C) 2010 RobotCub Consortium
# Authors: Ali Paikan, Massimo Regoli
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

import sys
import yarp

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

# read encoders
encs = yarp.Vector(ipos.getAxes())
ienc.getEncoders(encs.data())

print "Current encoders value: "
print encs.toString(-1, -1)

driver.close()

yarp.Network.fini()

