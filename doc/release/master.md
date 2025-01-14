YARP <yarp-3.11> (UNRELEASED)                                         {#yarp_3_11}
============================

[TOC]

YARP <yarp-3.11> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.10%22).

Fixes
-----

New Features
------------

### devices 

#### multiplenalogsensorremapper

* The angular acceleration and linear velocity values measured by a sensor can now be extracted and used via the sensor remapper.
* Also involves `multipleanalogsensorclient` and `multipleanalogsensorserver` as a breaking change.

### GUIs

#### `yarpopencvdisplay`

* `yarpopencvdisplay` is now able to display a `yarp::sig::LayeredImage`

### Libraries

#### `libYARP_sig`

* added new datatype `yarp::sig::LayeredImage`
* added `yarp::sig::utils::sum()` to transform `yarp::sig::LayeredImage` to `yarp::sig::Image`
