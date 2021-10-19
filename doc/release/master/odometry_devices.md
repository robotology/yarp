fakeOdometry {#master}
-----------------------

### devices
* `fakeOdometry` that generates a fake odometry and makes it available from `IOdometry2D` interface.

odometry2D_nws_yarp {#master}
-----------------------

### devices
* nws yarp that can be attached to a `IOdometry2D` interface and publish the data in 3 ports:
  * odometer
  * odometry
  * velocity

  the names are chosen by the user.

odometry2D_nws_ros {#master}
-----------------------

### devices
* nws ros that can be attached to a `IOdometry2D` interface and publish the data on a ros topic.

