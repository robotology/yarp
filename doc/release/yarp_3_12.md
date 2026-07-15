YARP <yarp-3.12> (UNRELEASED)                                         {#yarp_3_12}
============================

[TOC]

YARP <yarp-3.12> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.12%22).

## libYARP_sig

* added `Pose6D`, `ColorRGB` datatypes
* Improved the speed of reading MP3 files from disk

## libYARP_dev

* added `ISimulatedWorld` interface: an interface to control a simulation (e.g. Gazebo), enabling real-time creation and movement of objects.

## libYARP_os

* improved log colors

## devices

* added `simulatedWorld_nws_yarp` device
* added `simulatedWorld_nwc_yarp` device
* added `fakeSimulatedWorld` device
* enabled buffering for input port of `audioPlayerWrapper` device
* Fixed `FrameGrabberControls_Responder`/`Forwarder` silently reporting RPC
  transport success instead of the underlying device operation result for
  `setFeature`, `setActive`, `setMode`, and `setOnePush`. These calls now
  correctly return `false` when the device rejects the request, matching
  the existing `RgbVisualParams`/`DepthVisualParams` behavior (#3352).

## Bindings

* Added bindings for `yarp::dev::INavigation2D`, `yarp::dev::IMap2D`, `yarp::dev::ILocalization2D` interfaces
* Issue with bindings on and YARP_NO_MATH defined fixed (it was affecting the newly added bindings as ell as thealready present `yarp::dev::IFrameTransform` ones)
* Added bindings for `yarp::dev::ISimulatedWorld` interfaces

## Commands

### `yarpAudioPlayer`

* Added new executable `yarpAudioPlayer`

### `yarprobotinterface`

* The parser now generates the macro `${portprefixnoslash}` and `${robotname}`, starting from the `name` and `portprefix` attribute of the `robot`
  tag in the configuration file. The `${portprefix}` macro is unchanged. See the updated yarprobotinterface documentation for details.

## GUIs

### `yarpmanager`

* Added an option (to be passed as launch arg) to disable apps autoreload
```
> yarpmanager --disable_autoreload
```
