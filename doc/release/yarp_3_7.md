YARP <yarp-3.7> (UNRELEASED)                                         {#yarp_3_7}
============================

[TOC]

YARP <yarp-3.7> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.7%22).

Bug Fixes
---------

### CMake

* Fix compilation against graphviz 3 on Windows. To work correctly, the fix requires that YCM >= 0.14.2 is used.

* Migrate to use CMake's official `FindSQLite3.cmake` module.

### Libraries

#### `lib_yarp_dev`

* added missing timestamp to `IOdometry2D::getOdometry()` method

### Tools

#### `yarpmotorgui`

* This pr solves the problem reported in issue [#issue2839](https://github.com/robotology/yarp/issues/2839). Now `yarpmotorgui` does not show the error dialog when setting only a subset of the
available robot parts to a custom position


