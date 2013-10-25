#!/usr/bin/ruby

# Copyright: (C) 2013 iCub Facility
# Author: Paul Fitzpatrick
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

require 'yarp'

Yarp::Network.init()

p = Yarp::BufferedPortBottle.new()
p.open("/ruby")
p.close()
