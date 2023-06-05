YARP <yarp-3.8> (UNRELEASED)                                         {#yarp_3_8}
============================

[TOC]

YARP <yarp-3.8> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.8%22).

Bug Fixes
---------

### tools

#### `yarpmotorgui`

* Fixed an issue regarding high CPU usage (#2955).

### devices 

#### multipleanalogsensorsserver

Fixed bug that resulted in a segmentation fault if one of the device to which
`multipleanalogsensorsserver` was attached did not resized the measure vector.

#### serialPort_nws_yarp serialPort_nwc_yarp

Fixed segfault in serialPort_nws_yarp if devices is used but not attached yet.
Fixed communication with serialPort_nwc_yarp.

#### opencv_grabber

Removed use of highgui.h include file.
