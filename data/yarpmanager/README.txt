# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

Some quick unofficial notes on usage.

What is?
========
- Module, is an executable binary file developed using YARP
  or any other libraries;
- Application, referes to a collection of modules and connections
  or other applications.
- Resource refers to any physical or logical resources which are
  needed by a module. (e.g Memory, GPU, Platform)

See ./tests/templates to understand how to use XML model to describe
each of these concepts.


Configuration
=============
An example of yarpmanager configuration file can be found in
./tests/ymanager.ini.

'apppath', 'modpath', 'respath' respectively point to the folder where
the XML description of Applications, Modules and Resources are stored.
if 'load_subfolders' is enabled, applications description files will be
recursively loaded from subfolders of 'apppath'.

Set 'external_editor' to your preferable text editor. It will be used by
yarpmanager for editing any XML file.


Running examples
================
First make sure you have a name server running. Open a terminal and type:
$ yarpserver

using yarpmanager
=================

Open another terminal and try this:
$ cd ${YARP_ROOT}/src/yarpmanager/tests
$ yarpmanager
>> help
     # a list of yarpmanager keywords
>> list app
     # you should see EyesViewer-Localhost, FakeEyes, ...
>> load app EyesViewer-Localhost
>> run
>> connect
     # you should see moving red bar
>> disconnect
>> stop
>> exit


using yarpmanager (1.1)
==================
$ cd ${YARP_ROOT}/src/yarpmanager/tests
$ yarpmanager
+ From "Entities" frame, click the triangle beside Applications, then
  double-click "EyesViewer-Localhost".
+ From menu "Manage" press "Run" to run all modules
+ From menu "Manage" press "Connect" to stablish all connections.
+ To terminate application, chose "Manage">"Disconnect" and "Manage">"Stop".

To run "EyesViewer-Cluster" using load-balancer you need to have yarprun
running as server; Open a terminal and type:
$ yarprun --server /node1

Open another one and type:
$ yarprun --server /node2

Then try this:
$ yarpmanager
+ From "Entities" frame, click the triangle beside Applications, then
  double-click "EyesViewer-Cluster".
+ From the right side, right-click on the frame where you can see all
  modules and choose "Assign hosts". You will see that modules will be
  automatically assigned to available nodes. (here /node1 and /node2)
+ From menu "Manage" press "Run" to run all modules
+ From menu "Manage" press "Connect" to stablish all connections.
+ To terminate application, chose "Manage">"Disconnect" and "Manage">"Stop".


To see the system information of a node (e.g system load average), for
example of "/node1"

+ From "Entities" frame, click the triangle beside Resources, then
  double-click "node1".
+ From menu "Manage" press "Refresh Status" to read system information


Some Notes
==========
While making an application using XML description file, if you set <node></node>
to "localhost", the manager will use an internal module launcher to run the module
on localhost.

Leaving <node></node> empty, later you can use the load balancer of yarpmanager
to automatically assign module a a node which has less processor load. Notice that,
available nodes should be  listed using Resource XML file.
See ./xml/resources/cluster_nodes.xml for an example.
