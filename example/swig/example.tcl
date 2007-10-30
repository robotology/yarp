#!/usr/bin/tclsh

# You may have to fix this up, depending on your OS
# I don't really know anything about TCL
# Use .so version for linux, .dll version for windows
load ./cyarp.so cyarp
#load [file join [pwd] cyarp.dll] cyarp

Network_init

BufferedPortBottle p
p open "/tcl"

set top 100
for {set i 1} {$i<=$top} {incr i} {
    set bottle [p prepare]
    $bottle clear
    $bottle addString "count"
    $bottle addInt $i
    $bottle addString "of"
    $bottle addInt $top
    puts [concat "Sending " [$bottle toString]]
    p write
    Time_delay 0.5
}
p close

Network_fini

