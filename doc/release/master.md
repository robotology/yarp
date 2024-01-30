YARP 3.10.0 (XXXX-XX-XX)                                                {#v3_10_0}
==================

[TOC]

YARP 3.10.0 Release Notes
===================

A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+v3.10.0%22).

Major Behaviour Changes
---------------------------------

* ROS1 support removed from github repo https://github.com/robotology/yarp and moved to legacy repo https://github.com/robotology/yarp-ros
* Removed support for CMake 3.16/3.17. CMake minimum version is now 3.19

Deprecations and removals
---------------------------------
* Removed `xmlrpc` and `tcpros` carriers.
* Removed `yarpidl_rosmsg`.
* Removed library `libYARP_wire_rep_utils`.
* Removed `extern/md5`
* Removed `extern/ros` messages and examples.

Fixes
-----

* Configuration files installed by the `yarp_configure_plugins_installation` CMake macro are now relocatable (https://github.com/robotology/yarp/issues/2445, ).

New Features
------------

* Added new command line tool `yarpDeviceParamParserGenerator`. See official yarp documentation (cmd_yarpDeviceParamParserGenerator.dox)