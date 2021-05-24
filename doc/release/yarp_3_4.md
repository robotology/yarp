YARP <yarp-3.4> (UNRELEASED)                                         {#yarp_3_4}
============================

[TOC]

YARP <yarp-3.4> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.4%22).


Bug Fixes
---------

### Libraries

#### `sig`

##### `Image`

* Allocate the right amount of bytes for YUV422 images
* Decode vocabs when printing


### Devices

#### `BatteryWrapper`

* Data is now published on the yarp port even if some of the methods belonging
  to `IBattery` interface are not implemented (returning false).
  The only mandatory  method is `getBatteryStatus()`.
