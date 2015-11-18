#!/usr/bin/perl

# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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
