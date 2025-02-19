YARP <yarp-3.11> (UNRELEASED)                                         {#yarp_3_11}
============================

[TOC]

YARP <yarp-3.11> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.11%22).

Breaking Changes
----------------

### Library

#### `libYARP_dev`

* The following interfaces have been modified to use `yarp::dev::ReturnValue`:
  - `IRangeFinder2D`

### Devices

* The following devices haven been modified to use `yarp::dev::ReturnValue`:
  - `Rangefinder2DTranformer`
  - `FakeLaser`
  - `FakeLaserWithMotor`
  - `Rangefinder2D_nws_yarp`
  - `Rangefinder2D_nwc_yarp`
