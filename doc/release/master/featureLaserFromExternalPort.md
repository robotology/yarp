featureLaserFromExternalPort {#master}
----------------------------

### Libraries

#### `dev`

* Added new files: `Lidar2DDeviceBase.cpp` and `Lidar2DDeviceBase.h` which are a
  template for all lidar devices, e.g. `LaserFromExternalPort`.


### Devices

#### `LaserFromExternalPort`

* added `laserFromExternalPort` device, which is able to receive a `LaserScan2D`
  datatype from a YARP port, and expose it as a device driver through the
  `IRangefinder2D` interface.

