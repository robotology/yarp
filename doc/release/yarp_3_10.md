YARP <yarp-3.10> (UNRELEASED)                                         {#yarp_3_10}
============================

[TOC]

YARP <yarp-3.10> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.10%22).

Fixes
-----

### `bindings`

* Fixed regression bug in yarp.i: the file swig_python_windows_preable.i is no more included when a downstream 
project (like icub-main) includes the file in its own .i swig binding file. See https://github.com/robotology/yarp/pull/3148
