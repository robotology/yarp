#!/usr/bin/perl

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

use strict;
use warnings;

use yarp;

yarp::Network::init();

my $p = new yarp::BufferedPortBottle();

$p->open("/perl");

my $top = 100;
for (my $i=1; $i<=$top; $i++) {
    my $bottle = $p->prepare();
    $bottle->clear();
    $bottle->addString("count");
    $bottle->addInt32($i);
    $bottle->addString("of");
    $bottle->addInt32($top);
    print "Sending ", $bottle->toString(), "\n";
    $p->write();
    yarp::delay(0.5);
}

$p->close();

yarp::Network::fini();
