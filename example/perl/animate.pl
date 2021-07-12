#!/usr/bin/perl

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

use strict;
use warnings;

sub SafeSystem {
    my $cmd = shift;
    system($cmd) == 0
        or die "\nPROBLEM: This command failed - [$cmd]\n\n";
}

my $instruct = "The output of this script can be piped to the image viewer.
If the image viewer is registered with name /gcamview/i:img, here's how:

   \$ yarp name query /gcamview/i:img
   registration name /gcamview/i:img ip 127.0.0.1 port 10012 type tcp
   *** end of message

    \$ ./animate.pl | telnet 127.0.0.1 10012

You should see a moving gradient on the image viewer.
";

print STDERR "$instruct\n";

print "CONNECT /perl/animate\n";

my $h = 16;
my $w = 16;

for (my $i=0; $i<100; $i++) {
    print "d\n";
    my $all = $h*$w;
    my $r = $i/10;
    my $c = abs(cos($r));
    my $s = abs(sin($r));
    print "[mat] [mono] (1 $all 1 $w $h) {\\\n";
    for (my $y=0; $y<$h; $y++) {
        for (my $x=0; $x<$w; $x++) {
            my $v = (($x*$c+$y*$s)*10+$i)%256;
            print " $v";
        }
        print " \\\n";
    }
    print "}\n";
    SafeSystem("usleep 30000");
    print STDERR "img $i\n";
}

print "q\n";

print STDERR "$instruct\n";
