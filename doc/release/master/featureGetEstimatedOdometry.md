featureGetEstimatedOdometry {master}
----------------------
Yarp version is now 3.3.102

### YARP_dev

* Added method `yarp::dev::Nav2D::ILocalization2D::getEstimatedOdometry()` to the interface `ILocalization2D`.
  All devices using interface `ILocalization2D` updated accordingly to implemetent the new mandatory method.
* Added yarp::dev::Nav2D namespace to several devices related to 2D Navigation, e.g. `Navigation2DClient`, `Navigation2DServer`, `Localization2DClient`, 
  `Localization2DServer` , `fakeLocalizerDev`, `fakeNavigationDev`

### devices

#### Localization2DServer
* added missing ROS initialization in `Localization2DServer`. It will publish odometry data on ROS topic.

