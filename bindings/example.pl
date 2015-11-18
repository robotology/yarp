#!/usr/bin/perl

# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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
    $bottle->addInt($i);
    $bottle->addString("of");
    $bottle->addInt($top);
    print "Sending ", $bottle->toString(), "\n";
    $p->write();
    yarp::Time::delay(0.5);
}

$p->close();

yarp::Network::fini();
