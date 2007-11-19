#!/usr/bin/tclsh

global name_server

set name_server {"localhost" 10000}

proc send_text {target cmd} {

    # connect a socket to the target
    set host [lindex $target 0]
    set port [lindex $target 1]
    set sock [socket $host $port]
    fconfigure $sock -buffering line

    # make connection; request text mode with acknowledgements
    puts $sock "CONNACK tcl"
    set reply [gets $sock]
    # reply should be "Welcome tcl"
    if {$reply ne "Welcome tcl"} {
	puts "Warning: target is not behaving like a YARP port"
    }

    # send the command
    puts $sock "d"
    puts $sock $cmd

    # collect the reply
    set reply [gets $sock]
    set surplus ""
    while {$surplus ne "<ACK>"} {
	set surplus [gets $sock]
	if {$surplus ne "<ACK>"} {
	    set reply "$reply $surplus"
	}
    }

    return $reply
}

proc query {portname} {
    global name_server
    set r [send_text $name_server "query $portname"]
    if {$r eq "*** end of message"} {
	if {$reply ne "Welcome tcl"} {
	    puts "Warning: could not find port $portname"
	}
	return [list]
    } else {
	set contact_info [eval "list $r"]
	set result [list [lindex $contact_info 4] [lindex $contact_info 6]]
	puts "Found $portname at $result"
	return $result
    }
}

proc evaluate_bottle {txt} {
    set txt [string map {"\(" "\{"} "$txt"]
    set txt [string map {"\)" "\}"} "$txt"]
    set txt [string map {"\[" ""} "$txt"]
    set txt [string map {"\]" ""} "$txt"]
    set txt [string map {"\$" "_"} "$txt"]
    return [eval list $txt]
}

proc send_command {contact cmd} {
    set r [send_text $contact $cmd]
    return [evaluate_bottle $r]
}

set motor [query "/controlboard/rpc:i"]

puts [send_command $motor "get axes"]
puts [send_command $motor "set pos 0 50"]


