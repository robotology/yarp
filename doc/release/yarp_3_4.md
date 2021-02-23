YARP <yarp-3.4> (UNRELEASED)                                         {#yarp_3_4}
============================

[TOC]

YARP <yarp-3.4> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.4%22).


### Libraries

#### `robotinterface`

* Fixed handling of `attach` action for `IWrapper` devices.


## Devices

### `controlboardwrapper2`

* `getTorque` is no longer called twice.

### `fakeFrameGrabber`

* The `rand` mode was fixed.
* The timestamp is now correct.
* The image is no longer initialized when all the pixels are overwritten.

### `virtualAnalogWrapper`

* Fixed favor the use of parentheses with the keyword `networks` in the XML
  files. The old style is deprecated but still accepted.
  This is a fix in the sense that `virtualAnalogWrapper` was behaving
  differently from `ControlBoardWrapper`.


## GUI

### yarpmotorgui

* Fixed colors on dark mode desktop (#2466).
