YARP <yarp-4.0> (UNRELEASED)                                         {#yarp_4_0}
============================

[TOC]

YARP <yarp-4.0.> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.12%22).

New Features
----------------

### Devices

All nwc devices now use the method checkProtocolVersion() to guarantee network protocol 
compatibility with the corresponding nws device.

### libYARP_sig

Added VectorOf<float> (32 bit)


Deprecations and removals
---------------------------

### Devices

* Removed deprecated devices `analogWrapper`, `analogSensorClient`


Breaking Changes
----------------

### Devices

* Updated protocol between `RemoteControlBoard` and `controlBoard_nws_yarp`: the motor `temperature` data is
  now obtained through the streaming port instead of an RPC. 
  This new feature is implemented through a breaking change in the definition of data fields in `stateExt.thrift`

* `ISerialDevice` interface was refactored to use `yarp::dev::ReturnValue`. Some methods changed their signature
  to improve interface usability.
  Affected devices: `serialPort_nwc_yarp`, `serialPort_nws_yarp`, `fakeSerialPort`, `serialDeviceDriver`

### libYARP_sig

* Removed field `topIsLow` from `yarp::sig::Image`

### libYARP_os

* Removed classes `yarp::os::Node`,`yarp::os::Nodes`,`yarp::os::Publisher`,`yarp::os::Subscriber`
* Removed ROS1-related management logic in YARP nameserver