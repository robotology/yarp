featureLaserFromExternalPort {master}
----------------------

### YARP_dev

* Added new files: `Lidar2DDeviceBase.cpp` and `Lidar2DDeviceBase.h` which are a template for all lidar devices, e.g. LaserFromExternalPort

### devices

#### LaserFromExternalPort
* added `laserFromExternalPort` device, which is able to receive a LaserScan2D datatype from a Yarp port, and expose it as a device driver through the `IRangefinder2D` interface. 

