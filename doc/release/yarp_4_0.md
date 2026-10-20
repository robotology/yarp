YARP <yarp-4.0.2> (UNRELEASED)                                         {#yarp_4_0}
============================

[TOC]

YARP <yarp-4.0.2> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-4.0.2%22).

### libYARP_sig

* Added method `yarp::sig::VectorOf::toFormattedString(int precision, int width)`.

### libYARP_os

* Fixed signed overflow when `yarp::os::Stamp` wraps its sequence number to zero.

### Devices

#### `RGBDSensor_nwc_yarp`

* Fixed segfault due to a race condition in `RGBDSensor_StreamingMsgParser::read()`:
  the timestamps are now taken from the received data instead of calling `getEnvelope()` on the ports.

## Build system

* `YARP_SOVERSION` is now set to `${YARP_VERSION_MAJOR}` (it was previously hardcoded to 3).

## CI

* Improvements to CI workflows.

