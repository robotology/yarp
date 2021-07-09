#!/usr/bin/perl

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

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
