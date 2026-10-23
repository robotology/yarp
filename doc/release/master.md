YARP <yarp-4.1.0> (UNRELEASED)                                         {#yarp_4_1_0}
============================

[TOC]

YARP <yarp-4.1.0> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-4.1.0%22).

### libYARP_profiler

* Added tests for CI

### libYARP_os

* class `yarp::os::Property` is no more used for any network serialization routine.

### libYARP_dev

* added new thrift data types `DistortionModelData`, `IntrinsicParamsData` to replace the use of `yarp::os::Property` in `IRgbVisualParams.h`, `IDepthVisualParams.h` interfaces.
* Several methods of `MultipleAnalogSensor` interfaces migrated from returning `bool` to returning `yarp::dev::ReturnValue`

### thrift generator

* Updated thrift generator to use `yarp.api.include` also in `enums` when required.
* Updated thrift generator to add an error message if the conversion of an enum toString() or fromString() fails.
* Fixed code generation for nested map values: each container depth now uses its own map value name, so structures containing maps of maps or maps of lists of maps compile correctly.

### devices

* `controlBoard_nws_yarp`: exposed the `IControlMode` getters (`getControlModeOneRPC`, `getControlModeAllRPC`, `getControlModeGroupRPC`) on the RPC interface, so control modes can be read back via `yarp rpc` again (#3406).
* `ControlBoardRemapper` and `RemoteControlBoardRemapper` refactored to support ParamParser.
* `MultipleAnalogSensorClient` and `MultipleAnalogSensorServer` now use the new `IMultipleAnalogSensor` interfaces returning `yarp::dev::ReturnValue`

### bindings

* `LogComponent` added to bindings to improve yarp logging system integration
* `IRGBDSensor` Added missing include and extensions to fully use the IRGBDSensor interface with the bindings.
