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
* `yarpbatterygui` now compatible with battery_nwc_yarp.
* Fixed compilation of portmonitor carrier when a custom non-system swig is used
* Fixed compatibility with ffmpeg 7 (https://github.com/robotology/yarp/pull/3109).
* Fixed compilation with graphviz 10 (https://github.com/robotology/yarp/pull/3115).
* Fix sending empty yarp::sig::Vector when building in Debug (https://github.com/robotology/yarp/pull/3123).

New Features
------------

* Added new command line tool `yarpDeviceParamParserGenerator`. See official yarp documentation (cmd_yarpDeviceParamParserGenerator.dox)


#### Docker
* Added two parameters to yarp `Dockerfile`:
  * `base_img` to allow starting from different parent images
  * `yarp_branch` to set the yarp version the users might need in their image

### Carriers

* Removed h264 Carrier
* Added gstreamer carrier with extended functionalities.
* Added new gstreamers plugins: yarpvideosource, yarpvidepassthrough, yarpvideosink

### Devices

* Most yarp devices now use yarpDeviceParamParserGenerator to generate parameters documentation.

#### controlboardremapper

* Aligned to `controlBoard_nws_yarp` in terms of required interfaces. See https://github.com/robotology/yarp/pull/3095.

#### deviceBundler

* Added new device `deviceBundler` which can be useful to open two devices and attach them while using a single yarpdev command line.
  See https://github.com/robotology/yarp/discussions/3078

#### llmDevice

* Added LLM_Message data type to propagate LLM answers
* Added refreshConversation feature in the interface to allow users to restart the conversation mantaining the same prompt.

#### Navigation2D

* Added followPath functionality
* Added paths as possible arguments in gotoLocation in Navigation2D_nwc

#### Rangefinder2DTransformer

* Removed deprecated device Rangerfinder2DClient
* Added device Rangerfinder2DTransformer, with similar functionalities to Rangerfinder2DClient.

#### Rangefinder2D_nwc_yarp, Rangefinder2D_nws_yarp

* Network protocol now uses IDL thrift

#### ffmpeg_grabber

* Deprecated device

#### ffmpeg_writer

* Deprecated device

#### FakePythonSpeechTranscription

* Added new device `FakePythonSpeechTranscription`. The device is also an example which demonstrates the encapsulation of python code inside a c++ device implementing a Yarp interface. 

### GUIs

#### yarpopencvdisplay

* added new executable `yarpopencvdisplay`. Similarly to yarpview, it's a basic window to display video streams. It also allows to save to .avi files.

### Libraries

#### `lib_yarp_dev`

* `Drivers.cpp` If the user requests for a not existing plugin, the system now prints a message suggesting devices with similar names
* The following data types have been migrated from `yarp_dev` to `yarp_sig` library: `AudioPlayerStatus, AudioRecorderStatus, AudioBufferSize, AudioBufferSizeData, LaserMeasurementData, LaserScan2D`.

#### `lib_yarp_sig`

* Improvements to serialization class `yarp::sig::Sound` (breaking change)
* yarp_sig can now use IDL thrift to generate custom data types.
* The following data types have been migrated from `yarp_dev` to `yarp_sig` library: `AudioPlayerStatus, AudioRecorderStatus, AudioBufferSize, AudioBufferSizeData, LaserMeasurementData, LaserScan2D`.
