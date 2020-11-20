ci_GitHubActions {#yarp_3_4}
----------------

### Build System

* YCM is now searched when compiling bindings externally.
* Fixed dependency on `YARP_math` for several devices (`laserFromExternalPort`,
  `laserFromPointCloud`, `laserFromRosTopic`).
* `rpLidar2` device is now disabled when building with Clang.
* Fixed `upowerBattery` device in static builds.
