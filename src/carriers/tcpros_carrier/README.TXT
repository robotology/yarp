# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

Most of the knowledge in this README is now encapsulated in the "yarpros"
program.  Run it without arguments for a list of what is possible now.

ROS data is represented in YARP as blobs.  As a user, when you read or
write to a port, the data you'll get is exactly as ROS's message/service
types would suggest, preceded by a 12-byte header (see BlobHeader.h).
This header is "virtual", it is just for compatibility with the YARP
Bottle serialization format.

Reminders to myself of the state of this carrier...
End-users should wait until everything is neatly wrapped up :-)
  -paulfitz


Basic XML/RPC use for talking to ROS:
=====================================

Enable xmlrpc_carrier. This is experimental, but  sufficient to interoperate
with the XmlRpc++ library's "Hello" server and client.  Carrier name is
"xmlrpc".  XML structures are converted to Bottle-compatible form.

Here's a quick test against ROS, which uses XML/RPC for some
APIs. Assumes roscore and "talker.cpp" tutorial example running:

  yarp name register /roscore tcp localhost 11311
  echo "lookupNode dummy_id /talker" | yarp rpc xmlrpc://roscore
  [prints] 1 "node api" "http://contact:37291/"

  yarp name register /talker tcp ... 37291
  echo "requestTopic dummy_id /chatter ((TCPROS))" | yarp rpc xmlrpc://talker
  [prints] 1 "" (TCPROS contact 38265)

  yarp name register /talker/chatter tcp ... 38265

now we switch over to tcpros_carrier...

reading from ros publishers:
----------------------------

tcpros_carrier can read from ros publishers as follows.  Do the
"talker" XML/RPC example above, then:

  yarp read /read (or make a program with an input port)
  yarp connect /read /talker/chatter tcpros+topic./chatter

You'll then see the messages from /talker, but represented as an
uninterpreted list of numbers rather than a string.

Note that the connect should probably be accompanied by talking
to the roscore server to let it know there's a new publisher of
sorts; easy to do, but punting for now...

writing to ros subscribers:
---------------------------

Start a writer (or make a program with an output port):
  yarp write /write

Start the ros "listener" tutorial program
  ./listener

Start a test yarp rpc server, with which we will manually provide
the slave API for our "yarp write":
  yarp rpcserver /slave

Now let the yarp server know about the ROS listener:

  echo "lookupNode dummy_id /listener" | yarp rpc xmlrpc://roscore
  [prints] 1 "node api" "http://contact:56227/"

  yarp name register /talker tcp ... 56227

Now ask the listener to ask our fake slave where to connect
to [are you still following? :-)]

  yarp name query /slave
  [prints] registration name /write ip 192.168.1.2 port 10102 type tcp

  yarp rpc xmlrpc://listener
  [type] publisherUpdate dummy_id /chatter ("http://YOURHOSTNAME:10102")

At this point, on the termnal running "yarp rpcserver" you'll see:

  [prints] Message: requestTopic "/listener" "/chatter" ((TCPROS))
  [prints] Reply:

On another terminal check where our /write port is:

  yarp name query /write
  [prints] registration name /write ip 192.168.1.2 port 10015 type tcp

Now type this to "yarp rpcserver" and hit return:
  [type] 1 "" (TCPROS YOURHOSTNAME 10015)

Done!  The subscriber should now have connected with our /write
port, and be waiting for us to give it data.  Currently, we need
to give blob data.  On the "yarp write" terminal, do:
  [type] {4 0 0 0 97 32 97 0}

The 4-bytes string "a a" (with a null termination) should appear
on the listener terminal.  Actually I don't think ROS wants the
null.


writing to ros services
-----------------------

v1=`echo "lookupNode dummy_caller_id /add_two_ints_server" | yarp rpc xmlrpc://roscore | sed "s/.*://" | sed "s|/.*||"`
yarp name unregister /add_two_ints_server
yarp name register /add_two_ints_server tcp localhost $v1
v2=`echo "requestTopic dummy /add_two_ints ((TCPROS))" | yarp rpc xmlrpc://add_two_ints_server | sed "s/.* //" | sed "s/[^0-9]//"`
yarp name unregister /adder
yarp name register /adder tcp localhost $v2

 echo "{ 8 0 0 0   0 0 0 0   2 0 0 0   0 0 0 0}" | yarp rpc tcpros+service./add_two_ints://adder )
# get back { 10 0 0 0 0 0 0 0 }, which is 8 + 2
