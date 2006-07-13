#!/usr/bin/perl -w

use yarp;

yarp::Network::init();

my $p = new yarp::BufferedPortBottle();
#my $p = new yarp::BufferedPortImageRgb();

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
