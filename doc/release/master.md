YARP <yarp-3.11> (UNRELEASED)                                         {#yarp_3_11}
============================

[TOC]

YARP <yarp-3.11> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.11%22).

New Features
----------------

### Thrift

* Improved yarp thrift compiler to generate the following new methods for yarp services:
  - `getLocalProtocolVersion()`
  - `getRemoteProtocolVersion()`
  - `checkProtocolVersion()`
  These methods allow to test the compatibility in the network protocol for nws/nwc communication.
  To specify a new protocol version modify (or add if missing) the line `const i16 protocol_version = <xxx>` in the .thrift file. 
  See `fakeTestMsgs.thrift` and `FakeDevice_nwc_yarp` for an example.
 
### Library

#### `libYARP_os`

* Added new portable datatype `yarp:os::ApplicationNetworkProtocolVersion` 

### Devices

* Added new devices:
  - `FakeDevice_nws_yarp`
  - `FakeDevice_nwc_yarp`
 `FakeDevice_nwc_yarp` also uses the new `checkProtocolVersion()` to verify the communication with `FakeDevice_nws_yarp`

Breaking Changes
----------------

#### `libYARP_dev`

* The following interfaces have been modified to use `yarp::dev::ReturnValue`:
  - `IRangeFinder2D`
  - `ILLM`
  - `IChatBot`

### Devices

* The following devices haven been modified to use `yarp::dev::ReturnValue`:
  - `Rangefinder2DTranformer`
  - `FakeLaser`
  - `FakeLaserWithMotor`
  - `Rangefinder2D_nws_yarp`
  - `Rangefinder2D_nwc_yarp`
  - `fakeChatBotDevice`
  - `chatBot_nws_yarp`
  - `chatBot_nwc_yarp`
  - `fakeLLMDevice`
  - `LLM_nwc_yarp`
  - `LLM_nws_yarp`
