#!/usr/bin/perl

# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

use strict;
use warnings;

use yarp;

# set up a description of the device we want - just the fake framegrabber
my $prop = new yarp::Property();
$prop->put("device","test_grabber");
$prop->put("width", 640);
$prop->put("height", 480);
print "Property is ", $prop->toString(), "\n";

# create the device
my $dev = new yarp::PolyDriver($prop);

# get the "IFrameGrabberImage" interface
my $grabber = $dev->viewFrameGrabberImage();

# grab 30 images
my $img = new yarp::ImageRgb();
for (my $i=0; $i<30; $i++) {
    $grabber->getImage($img);
    my $w = $img->width();
    my $h = $img->height();
    print "Got a ${w}x${h} image\n";
}

# shut things down
$dev->close();
