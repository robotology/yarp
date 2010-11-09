#!/usr/bin/python

# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

import yarp

yarp.Network.init()

class DataProcessor(yarp.PortReader):
    def read(self,connection):
        print("in DataProcessor.read")
        if not(connection.isValid()):
            print("Connection shutting down")
            return False
        bin = yarp.Bottle()
        bout = yarp.Bottle()
        print("Trying to read from connection")
        ok = bin.read(connection)
        if not(ok):
            print("Failed to read input")
            return False
        print("Received [%s]"%bin.toString())
        bout.addString("Received:")
        bout.append(bin)
        print("Sending [%s]"%bout.toString())
        writer = connection.getWriter()
        if writer==None:
            print("No one to reply to")
            return True
        return bout.write(writer)

p = yarp.Port()
r = DataProcessor()
p.setReader(r)
p.open("/python");

yarp.Time.delay(100)
print("Test program timer finished")

yarp.Network.fini();
