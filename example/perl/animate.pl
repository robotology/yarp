#!/usr/bin/perl -w

use strict;

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

    \$ ./animate.pl | telnet 127.0.0.1 100012

You should see a moving gradient on the image viewer.
";

print STDERR "$instruct\n";

print "CONNECT /perl/animate\n";

my $h = 8;
my $w = 8;

for (my $i=0; $i<100; $i++) {
    print "d\n";
    my $all = $h*$w;
    my $r = $i/10;
    my $c = abs(cos($r));
    my $s = abs(sin($r));
    print "[img] (1 1 $all $w $h $w) {\\\n";
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
