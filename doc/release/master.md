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
* The syntax yarpdev --device xxx --subdevice yyy has been deprecated. See discussion: https://github.com/robotology/yarp/discussions/3078

Fixes
-----

* Configuration files installed by the `yarp_configure_plugins_installation` CMake macro are now relocatable (https://github.com/robotology/yarp/issues/2445, ).
* Improved `ffmpeg` port monitor to allow using different couples of coders/decodes

New Features
------------

* Added new command line tool `yarpDeviceParamParserGenerator`. See official yarp documentation (cmd_yarpDeviceParamParserGenerator.dox)

* Added LLM_Message data type to propagate LLM answers

#### Docker
* Added two parameters to yarp `Dockerfile`:
  * `base_img` to allow starting from different parent images
  * `yarp_branch` to set the yarp version the users might need in their image

### Devices

#### controlboardremapper

* Aligned to `controlBoard_nws_yarp` in terms of required interfaces. See https://github.com/robotology/yarp/pull/3095.

#### deviceBundler

* Added new device `deviceBundler` which can be useful to open two devices and attach them while using a single yarpdev command line.
  See https://github.com/robotology/yarp/discussions/3078
