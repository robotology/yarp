YARP devel (UNRELEASED)                                                {#v3_2_0}
=======================

[TOC]

YARP devel Release Notes
========================


Important Changes
-----------------


New Features
------------

### Libraries

#### `YARP_dev`

* Added IFrameSource and IFrameSet interfaces in order to refactor the frame transform software stack.
* Added ImplementIFrameSource, a default implementation to reuse the mathematical part of IFrameSource in several devices.

### Devices

* Added `FrameReceiver` device
* Added `FrameBroadcaster` device
* Refactored `OVRDevice` with the new FrameTransform stack

Contributors
------------

This is a list of people that contributed to this release (generated from the
git history using `git shortlog -ens --no-merges v3.1.0..v3.2.0`):

```
```
