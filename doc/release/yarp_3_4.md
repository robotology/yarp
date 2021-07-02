---
doc/release/yarp_3_4/000_yarp_3_4.md
---


YARP <yarp-3.4> (UNRELEASED)                                         {#yarp_3_4}
============================

[TOC]

YARP <yarp-3.4> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.4%22).


Bug Fixes
---------

## Build System

* The `yarpros` tool is no longer compiled when `YARP_COMPILE_EXECUTABLES` is
  disabled. (#2598)


## Libraries

### `sig`

#### `Image`

* Fixed compatibility with images with topIsLow==false sent by YARP 3.5 (#2612).

### `dev`

#### `DeviceResponder`

* Removed the debug messages flooding (#2630, #2631).

## GUIs

### `yarpview`

* Fixed upside down image when topIsLow is false (Qt >= 5.3.2 only) (#2612).


### Devices

#### `localization2DClient`

* Implemented missing method `getEstimatedOdometry()`.

#### `navigation2DClient`

* Implemented missing method `getEstimatedOdometry()`,

#### `localization2DServer`

* Implemented RPC.
