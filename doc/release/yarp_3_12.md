YARP <yarp-3.12> (UNRELEASED)                                         {#yarp_3_12}
============================

[TOC]

YARP <yarp-3.12> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.12%22).

New Features
----------------

### yarpRerun
* Added new executable tool `yarpRerun`.
The tool log data received on yarp port using rerun.io framework)

### yarpRobotDescriptionInfo

* Added new executable `yarpRobotDescriptionInfo`.
This tool generate a .dot file which describes a graph representing all the devices used by yarpobotinterface
and their connections.

### Library

#### `libYARP_os`

* LogForwarder: added new method `getLogPortName()`
* Added new test `LogForwarderTest` to check that all forwarded messages are caught by the logger engine.

### Portmonitor

* Added new portmonitor `simulated_network_delay` to add a delay in the network  communication.
* Used by `LogForwarderTest` to test the logger under various network delay conditions.

#### `libYARP_dev`

* Added new tests to check `IImpedanceControl` interface.

Fixes
----------------

### Bindings

* Added bindings for `IJointCoupling` interface.
* Added bindings for `yarp::dev::LLM_Message`.
* Added bindings for `yarp::dev::ReturnValue`.
* Fixed bindings for Python callbacks. Added examples.
* Strip trailing newline from `CMAKE_INSTALL_PYTHON3DIR`.

### Executables

* `yarpActionsPlayer` Fixed erratic behavior during the first movement of each sequence.

### GUIs

#### yarpmotorgui

* Improved `yarpmotorgui` to handle `forceOffset` of `IImpedanceControl` interface: the value is sent only if the table has changed.

### Devices

* `ControlBoardRemapper`: fixed wrong buffer resize in `resizeSubControlBoardBuffers`.

### Library

#### `libYARP_os`

* `LogForwarder`: now using a separated thread (`class ThreadedPort`) to prevent the loss of log messages during stress condition.

#### `libYARP_robotinterface`

* `libYARP_robotinterface`: does not show error message if device does not derive from `IDeviceDriverParams`.

Breaking Changes
----------------

### GUIs

#### yarpmotorgui

* Add timers and callbacks for `pwm` and `current` control to keep sending references.

### Library

#### `libYARP_dev`

* `SensorMeasurments` datatype moved from `multipleAnalogSensorsMsgs` to lib `libYARP_dev`
