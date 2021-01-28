#!/usr/bin/python

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

import socket, re, sys

if len(sys.argv)!=2:
    print 'Call as:\n  %s /port/to/read/from'%sys.argv[0]
    exit(1)

try:
    import find_name_server
    name_server = find_name_server.find_name_server()
    print "Nameserver is here:", name_server
except:
    name_server = ('localhost',10000)
    print "Assuming nameserver is here:", name_server

def get_text(sock): # get a single line of text from a socket
    result = ""
    while result.find('\n')==-1:
        result = result + sock.recv(1024)
    result = re.sub('[\r\n].*','',result)
    return result
def get_address(s): # extract address from a name server response
    m = re.match("registration name [^ ]+ ip ([^ ]+) port ([0-9]+) type tcp",s)
    return (m.group(1),int(m.group(2))) if m else None
def send_message(addr,message): # send a message and expect a reply
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(addr)
    sock.send('CONNACK extern\n')
    get_text(sock)
    sock.send('d\n%s\n' % message)
    result = get_text(sock)
    sock.close()
    return result

def read_loop(addr):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(addr)
    sock.send('CONNECT extern\nr\n')
    while True:
        data = sock.recv(1024)
        if not data: break
        print data,

port_name = sys.argv[1]
read_loop(get_address(send_message(name_server,"query %s"%port_name)))
