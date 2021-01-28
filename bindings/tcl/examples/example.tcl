#!/usr/bin/tclsh

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# You may have to fix this up, depending on your OS
# I don't really know anything about TCL
# Use .so version for linux, .dll version for windows
load ./libtclyarp.so yarp
#load [file join [pwd] cyarp.dll] cyarp

Network_init

BufferedPortBottle p
p open "/tcl"

set top 100
for {set i 1} {$i<=$top} {incr i} {
    set bottle [p prepare]
    $bottle clear
    $bottle addString "count"
    $bottle addInt32 $i
    $bottle addString "of"
    $bottle addInt32 $top
    puts [concat "Sending " [$bottle toString]]
    p write
    Time_delay 0.5
}
p close

Network_fini
