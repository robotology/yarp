YARP <yarp-4.1.0> (UNRELEASED)                                         {#yarp_4_1_0}
============================

[TOC]

YARP <yarp-4.1.0> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-4.1.0%22).

Important Changes
-----------------

### Breaking changes

* The port administrative protocol has been reimplemented using the thrift service `PortCoreMsgs`.
  The old hand-written admin commands (e.g. `[add]`, `[del]`, `[ver]`, `[get]`, `[set]`, `list in`, `list out`, ...)
  have been replaced by thrift methods. As a consequence, ports of YARP 4.1 cannot be administered
  (e.g. `yarp connect`, `yarp disconnect`, `yarp ping`, qos settings) by older YARP versions and vice versa.
* `yarp::os::Property` does not derive anymore from `yarp::os::Portable` and it can no longer be
  serialized/transmitted over the network. All the occurrences where a `Property` was transmitted
  over the network have been replaced by custom thrift generated messages.
* `yarp::os::Header` has been renamed to `yarp::os::StampWithFrame` (now thrift generated).
* Serialization of `yarp::sig::Vector` (`yarp::sig::VectorOf<T>`) into a `yarp::os::Bottle` changed:
  a vector inside a bottle now requires parenthesis, i.e. `(1 2 3)`.
* The wire format of several data types changed, since they are now serialized using thrift
  (see the list below). Data exchanged with YARP 4.0 may not be compatible.
* Ruby SWIG bindings have been removed.
* Several methods of `IMultipleAnalogSensor` interfaces now return `yarp::dev::ReturnValue` instead of `bool`.

### libYARP_profiler

* Added tests for CI

### libYARP_os

* class `yarp::os::Property` is no more used for any network serialization routine.
  * `yarp::os::Property` does not derive anymore from `yarp::os::Portable` (methods `read()`/`write()` removed).
  * Removed methods `acquireProperties()` and `releaseProperties()` from `yarp::os::Contactable`
    (and from `yarp::os::Port`, `yarp::os::BufferedPort`, `yarp::os::AbstractContactable`).
  * Removed `yarp::os::Bottle::addDict()`, `BOTTLE_TAG_DICT`, `yarp::os::Value::isDict()`,
    `yarp::os::Value::asDict()`, `yarp::os::Value::asSearchable()`.
  * The protocol between `Network` / `NetworkProfiler` (client side) and `PortCore` (server side)
    has been revised to avoid the usage of `yarp::os::Property`. Added thrift data types
    `PortInfoData`, `ThreadInfoData`, `ConnectionQosData`.
* All the `PortCore` administrative commands are now implemented through the thrift service `PortCoreMsgs`
  (`addConnection`, `delConnection`, `getAdminProtocolVersion`, `listConnectionsIn/Out`, `getConnectionInfoIn/Out`,
  `getConnectionParamsIn/Out`, `setConnectionParamsIn/Out`, `getPortmonitorParamsIn/Out`, `setPortmonitorParamsIn/Out`,
  `attach/detachPortmonitorIn/Out`, `getPortInfo`, `get/setConnectionQosIn/Out`, `setProcessScheduling`, ...).
  The `help` command lists all the available commands.
* `yarp::os::Header` renamed to `yarp::os::StampWithFrame`, now generated from `StampWithFrameData.thrift`.
* Improved serialization of `yarp::os::Stamp` using thrift (`StampData.thrift`).
* Improved serialization of `yarp::os::SystemInfo` using thrift (`SystemInfoData.thrift`). `SystemInfoSerializer.h` has been removed.
* Fixed bug in `PortCoreInputUnit` and use of an uninitialized variable (`optlen`).
* Fixed possible memory leak in `DgramTwoWayStream`.

### libYARP_sig

* Refactored class `yarp::sig::Vector` (`yarp::sig::VectorOf<T>`) using thrift (`VectorOfData.thrift`).
  `Vector.h` has been divided into `VectorBase.h` and `Vector.h`.
* Vector serialization inside a bottle now requires parenthesis, i.e. `(1 2 3)` (breaking change).
* Added method `toFormattedString(int precision, int width)` to `yarp::sig::VectorOf<float>` and `yarp::sig::VectorOf<double>`.
* Improved serialization of `yarp::sig::Matrix` using thrift (`MatrixData.thrift`).
* `yarp::sig::PointCloud` does not depend anymore on `yarp::sig::VectorOf<T>`.
* Improved serialization of `yarp::sig::LayeredImage` and of its components using thrift.
  Added data types `ImageAlphaData`, `ImageColorKeyData`, `ImageLayerData`, `LayeredImageData`
  and headers `ImageColorKey.h`, `ImageLayer.h`.
* Improved serialization of `yarp::sig::SoundMarker` using thrift (`SoundMarkerData.thrift`).
* Added thrift data types `QuaternionData`, `TranslationData`, `Vec2DData`.

### libYARP_math

* Improved serialization of `yarp::math::FrameTransform` using thrift (`FrameTransformData.thrift`).
* Improved serialization of `yarp::math::Vec2D` template using thrift.
* Added header `yarp/math/Translation.h`.

### libYARP_dev

* added new thrift data types `DistortionModelData`, `IntrinsicParamsData` to replace the use of `yarp::os::Property` in `IRgbVisualParams.h`, `IDepthVisualParams.h` interfaces.
* added new thrift data types `CameraConfigData`, `VertexData` to improve the serialization of `IFrameGrabberImage` and `IRgbVisualParams` related data types.
* Several methods of `MultipleAnalogSensor` interfaces migrated from returning `bool` to returning `yarp::dev::ReturnValue`
* New method for `ISpeechSynthesizer`: `getAvailableVoices` to get a list of the available voices names for the current language

### libYARP_serversql

* Removed the `--ros` option of `yarpserver` (delegation of pub/sub to the ROS name server) and all
  the related delegate code (`NameServerContainer::whereDelegate()`, `setDelegate()`/`getDelegate()`
  in `Subscriber` and `NameServiceOnTriples`). The option had no effect since the removal of `RosNameSpace`.

### libYARP_manager

* Added application parameters: an application can use `$PAR{name}` placeholders, declare their default values with
  `<def-param-value name="name">default</def-param-value>`, and the applications
  embedding it can set their values with `<app-param name="name">value</app-param>`, so the same application can be embedded
  several times with different values. See the yarpmanager documentation for details.
* Fixed `Application::clear()` not clearing the arbitrators, which leaked from one application file to the next when loading a directory.

### yarpmanager

* The builder warns before saving an application that uses application parameters, and shows an error when an embedded
  application cannot be added.

### thrift generator

* Updated thrift generator to use `yarp.api.include` also in `enums` when required.
* Updated thrift generator to add an error message if the conversion of an enum toString() or fromString() fails.
* Fixed code generation for nested map values: each container depth now uses its own map value name, so structures containing maps of maps or maps of lists of maps compile correctly.
* `yarp.api.keyword` can now be used also on services, and not only on data types.
* Added support for protected/private data members (default: public).
* Fixed handling of `float32` and `size_t` data types.

### devices

* `controlBoard_nws_yarp`: exposed the `IControlMode` getters (`getControlModeOneRPC`, `getControlModeAllRPC`, `getControlModeGroupRPC`) on the RPC interface, so control modes can be read back via `yarp rpc` again (#3406).
* `ControlBoardRemapper` and `RemoteControlBoardRemapper` refactored to support ParamParser.
* `MultipleAnalogSensorClient` and `MultipleAnalogSensorServer` now use the new `IMultipleAnalogSensor` interfaces returning `yarp::dev::ReturnValue`
* Updated `ISpeechSynthesizerMsgs.thrift` and correspondent generated code for the new method `getAvailableVoices`
* Updated devices related to `yarp::dev::ISpeechSynthesizer` for the new method `getAvailableVoices`:
  * `FakeSpeechSynthesizer`
  * `SpeechSynthesizer_nws_yarp`
  * `SpeechSynthesizer_nwc_yarp`
* `frameGrabber_nws_yarp`, `frameGrabber_nwc_yarp`, `frameGrabberCropper`, `fakeFrameGrabber` updated to use the new thrift data types.

### carriers

* `shmem`: fixed a hang on Windows when closing a connection with replies (e.g. rpc).
* `shmem`: fixed the access mutex not being released when writing to a closed stream.
* `shmem`: added tests for CI.

### Commands

* Removed `yarp pray` command.

### GUIs

#### `yarpmanager`

* The cluster manager now supports also nodes running inside docker containers, conda and pixi environments.


### bindings

* `LogComponent` added to bindings to improve yarp logging system integration
* `IRGBDSensor` Added missing include and extensions to fully use the IRGBDSensor interface with the bindings.
* Added bindings for `IRgbVisualParams` and `IDepthVisualParams`.
* `yarp.i` has been split into multiple files: `yarp_os.i`, `yarp_sig.i`, `yarp_sig_vector.i`, `yarp_dev.i`.
* Removed Ruby bindings.
* Fixed compilation with SWIG 4.5.

### Build system

* Enabled parallel installation (`INSTALL_PARALLEL`) when using CMake >= 3.30.
