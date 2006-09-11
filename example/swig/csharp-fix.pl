#!/usr/bin/perl -w

use strict;

# for Mono compile
my $compile_command = "mcs *.cs";

my $txt = `$compile_command 2>&1`;

my @bugs = ($txt =~ /([a-z0-9_]+\.cs\([0-9]+),[0-9]+\).*CS0115/img);

foreach my $bug (@bugs) {
    if ($bug =~ /(.*)\((.*)/) {
	my $fileName = $1;
	my $lineNumber = $2;
	print "Problem at $fileName:$lineNumber ... ";
	
	my $buffer = "";
	my $ct = 1;
	open(FIN,"<$fileName");
	while (<FIN>) {
	    if ($ct==$lineNumber) {
		$_ =~ s/override/virtual/;
		print " fixed";
	    }
	    $buffer .= $_;
	    $ct++;
	}
	print "\n";
	close(FIN);
	open(FOUT,">$fileName");
	print FOUT $buffer;
	close(FOUT);
    }
}


