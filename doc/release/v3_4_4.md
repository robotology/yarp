YARP <yarp-3.4> (UNRELEASED)                                         {#yarp_3_4}
============================

[TOC]

YARP <yarp-3.4> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.4%22).


Bug Fixes
---------

### Build System

* Fixed use of `CMAKE_INSTALL_PYTHON3DIR` CMake variable to specify the
  installation path of Python bindings, as in previous versions the variable
  was defined but ignored (#2523).
* Fixed the build of Python bindings on Windows (#2525, #2527).


### Libraries

### `conf`

* Fixed float128_t where long double is 64 bit (#2510).

#### `os`

* If in a `yarp::os::NetworkClock` a clock reset is detected, fill the gap
  between the waiter and the time published by the network clock port.
  A network clock reset is defined as a jump in the past of the time published
  by the network clock port.
  This fix avoids that all the threads that are waiting a
  `yarp::os::NetworkClock::delay` call on that network clock remain blocked when
  a time reset occurs  (#800, #2494).
* Fixed error in race condition during the scan of plugins in
  `YarpPluginSelector`, avoided concurrent access to variables (#2538).


### Tools

#### `yarplogger`

* Fixed crash when attempting to clear a selected log with its log tab opened
  (#2554).


### Bindings

#### Python

* Added `example_callback.py`, which showcases a `BufferedPort` with an attached
  callback handler for logging incoming bottles (#2555).
