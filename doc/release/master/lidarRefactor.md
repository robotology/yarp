lidarRefactor {#master}
-----------------------

### yarp::dev
* `laser2DDeviceBase` now implements `yarp::dev::IPreciselyTimed`
* all lidar devices must implement the method `acquireDataFromHW()` which is pure virtual in `laser2DDeviceBase`
* all lidar devices can call (but it is optional) the method `updateLidarData` of `laser2DDeviceBase`, which automatically calls `acquireDataFromHW()` + other utility functions (e.g. updateTimestamp(), etc.)
* the run() method of the devices, which was responsible for the data collection, now just calls `updateLidarData()`

### devices
* The following devices have been updated: `fakeLaser`,`laserFromDepth`,`laserFromExternalPort`,`laserFromPointCloud`,`laserFromRosTopic`,`rpLidar2`




