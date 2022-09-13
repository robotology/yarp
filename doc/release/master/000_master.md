YARP <yarp-3.8> (UNRELEASED)                                         {#yarp_3_8}
============================

[TOC]

YARP <yarp-3.8> Release Notes
=============================

A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.8%22).


Deprecation and Behaviour Changes
---------------------------------

### `Devices`

* Removed devices rpLidar, rpLidar2, rpLidar3, and the corresponding sdk in extern/rplidar.
  The devices can be now found in https://github.com/robotology/yarp-device-rplidar

New Features
------------

### Libraries

#### `lib_yarp_os`

* `yarp::os::LogStream` now can chain instances of `yarp::sig::VectorOf<T>`.
