YARP <yarp-3.11> (UNRELEASED)                                         {#yarp_3_11}
============================

[TOC]

YARP <yarp-3.11> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.10%22).

Breaking Changes
----------------

### Library

#### `libYARP_dev`

The signature of methods `IFrameTransform::frameExists(const std::string &frame_id)` and `IFrameTransform::canTransform(const std::string &target_frame, const std::string &source_frame)` change to `IFrameTransform::frameExists(const std::string &frame_id, bool& exits)` and `IFrameTransform::canTransform(const std::string &target_frame, const std::string &source_frame, bool& exists)`.

### Tests

The set of tests for individual plugins, previously called `harness_dev_xxx` has been renamed to `harness_devices_xxx`

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

#### `yarpmanager`
  * Added time info to `libYARP_manager` logger and to `yarpmanager` GUI.
  * The time info can be displayed as the current time (obtained from `std::chrono::system_clock::now()`) or as time elapsed since yarpmanager GUI launch

### Libraries

#### `libYARP_sig`

* added new datatype `yarp::sig::LayeredImage`
* added `yarp::sig::utils::sum()` to transform `yarp::sig::LayeredImage` to `yarp::sig::Image`
* modified signature of method `yarp::sig::utils::depthToPC` and `yarp::sig::utils::depthRgbToPC`.
  They now accept step_x and step_y parameters to perform pointcloud decimation.
  They also accept a new parameter `output_order` which allows to swap the axis of the output point cloud (see code documentation)

#### `libYARP_dev`

* added new class `yarp::dev::ReturnValue`
* The following interfaces have been modified to the new class ReturnValue:
  ISpeechSynthesizer
  ISpeechTranscription
  ILocalization2D
  IMap2D
  INavigation2D
  IOdometry2D

#### `devices`

* Updated all devices which use the interfaces employing the new class `ReturnValue`:
  FakeSpeechSynthesizer
  FakeSpeechTranscription
  FakeNavigation
  FakeLocalizer
  FakeOdometry2D
  Map2DStorage
  SpeechTranscription_nws_yarp
  SpeechTranscription_nwc_yarp
  SpeechSynthesizer_nws_yarp
  SpeechSynthesizer_nwc_yarp
  Localization2D_nws_yarp
  Localization2D_nwc_yarp
  Map2D_nws_yarp
  Map2D_nwc_yarp
  MobileBaseVelocityControl_nws_yarp
  MobileBaseVelocityControl_nwc_yarp
  Navigation2D_nwc_yarp
  Navigation2D_nws_yarp
  Odometry2D_nws_yarp

### Other

* Added new CLI executable yarpActionPlayer to playback trajectories on robot.
  See the related documentation included in the README.md file