fix_frameTransformSet_nws_ros_timestamp_value {#yarp_3_5}
-------------------

### Devices

#### `frameTransformSet_nws_ros`

* Fixed bug in `frameTransformSet_nws_ros::yarpTransformToROSTransform`. The time stamps of the static transforms were wrongly set to yarp::os::Time::now()
