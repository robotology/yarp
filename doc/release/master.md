YARP 3.9.0 (2023-11-21)                                                {#v3_9_0}
=======================

[TOC]

YARP 3.9.0 Release Notes
========================

A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+v3.9.0%22).

Major Behaviour Changes
---------------------------------

* Yarp now supports C++20 standard.
* Catch2 library updated to version: 3.4.0
* YCM library updated to 0.15.3

Deprecations and removals
---------------------------------
* Removed yarp companion command TopicTest.
* Removed unnecessary rosmsg dependencies in yarpdataplayer.
* Removed UNMAINTAINED executable yarpfs.
* Removed UNMAINTAINED library libyarpc.
* Removed deprecated device controlboardwrapper2.

Fixes
-----
* General improvement to yarp github CI to support Ubuntu22.04+clang17, Windows2022+VS17
* Fixed segfault in `audioRecorderDeviceBase` device.
* Added missing doxygen tags to several interfaces and devices.
* Fixed segfault when reading a mono .wav file from disk in `SoundFileWav.cpp`.
* Fixed stdin in `yarprun --cmd <cmd> --stdio`.
* Removed robot and `iCubGui` inertial ports from the example app as the GUI is unable to handle the MAS client.
* Fix bug that forces images to be sent through RPC calls in `frameGrabber_nwc_yarp`.
* The error message upon inconsistent encoder timestamps has been throttled in `controlBoard_nws_yarp`.
* Fixed issue regarding windows/linux timestamps synchronization: `yarp::os::SystemClock::nowSystem()` now maps to `std::chrono::system_clock` instead of `std::chrono::high_resolution_clock`

New Features
------------

### Docker

Added new experimental docker image for Ubuntu22.04 http://github.com/robotology/yarp/docker

### GUIs

#### yarpaudiocontrolgui

Added new gui `yarpaudiocontrolgui` to start/stop an `audioRecorder_nwc_yarp` or an `audioPlayer_nwc_yarp` device.

#### yarpllmgui

Added new gui `yarpllmgui` to control `IChatbot` devices.

### Libraries

#### libYARP_sig

* The class `yarp::sig::sound` has refactored to avoid the internal use (private implementation) of `yarp::sig::Image` data type.

#### libYARP_companion

* Added companion command `yarp split`. The command splits an heterogeneous nested bottle received from a port into multiple ports.

####  libYARP_dev

* Added new interface: `IJointCoupling`, an interface for handling coupled joints.
* Added new interfaces: `ISpeechSynthesizer`, `ISpeechTranscription`.
* Added new interface: `IChatBot` to interact with chatbots.
* The interface `ITorqueControl` now supports four a new friction parameters: `velocityThresh`.
  These parameters are used to enable the new torque control law and fine tune the the friction compensation at FW level.

### Devices

#### frameTransformStorage + frameTransformClient + frameTransformServer

* Added a timeout related parameter (`FrameTransform_container_timeout`) to change the refresh interval for old timed frame transforms.
* The parameter has also been added to the `frameTransformClient` and `frameTransformServer` configuration files. The `extern-name` for this parameter is:
 `ftc_storage_timeout` for `frameTransformClient` files; `fts_storage_timeout` for `frameTransformServer` files

#### chatBot

* Added nwc device `chatBot_nwc_yarp` for the `IChatBot` interface and the corresponding test
* Added nws device `chatBot_nws_yarp`
* Added fake device `fakeChatBotDevice`

#### speechSynthesizer

* Added nwc device `speechSynthesizer_nwc_yarp` for the `ISpeechSynthesizer` interface and the corresponding test
* Added nws device `speechSynthesizer_nws_yarp`
* Added fake device `fakeSpeechSynthesizer`

#### speechTranscription

* Added nwc device `speechTranscription_nwc_yarp` for the `ISpeechTranscription` interface and the corresponding test
* Added nws device `speechTranscription_nws_yarp`
* Added fake device `fakeSpeechTranscription`
