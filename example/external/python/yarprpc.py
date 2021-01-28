#!/usr/bin/python

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

import socket
import re
import sys

if len(sys.argv)!=3:
    print 'Call as:\n  %s /port/to/write/to \"message to send\"'%sys.argv[0]
    exit(1)

try:
    import find_name_server
    name_server = find_name_server.find_name_server()
    print "Nameserver is here:", name_server
except:
    name_server = ('localhost',10000)
    print "Assuming nameserver is here:", name_server

port_name = sys.argv[1]
message = sys.argv[2]

def get_addr(s):
    m = re.match("registration name [^ ]+ ip ([^ ]+) port ([0-9]+) type tcp",s)
    return (m.group(1),int(m.group(2))) if m else None

# get a single line of text from a socket
def getline(sock):
    result = ""
    while result.find('\n')==-1:
        result = result + sock.recv(1024)
    result = re.sub('[\r\n].*','',result)
    return result

# send a message and expect a reply
def comm(addr,message):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(addr)
    sock.send('CONNACK extern\n')
    getline(sock)
    sock.send('d\n%s\n' % message)
    result = getline(sock)
    sock.close()
    return result

# ask name server for location
query = get_addr(comm(name_server,"query %s"%port_name))
print "Talking to", port_name, "here:", query
print comm(query,message)
